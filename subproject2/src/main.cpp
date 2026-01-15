// 掌握C++多线程框架程序的开发
// 并可以通过YAML格式配置文件配置成前台或守护进程运行

#include <iostream> // 输入输出流，用于控制台打印
#include <vector>   // 动态数组容器，存储线程对象
#include <thread>   // C++11线程库，创建和管理线程
#include <chrono>   // 时间库，处理时间间隔
#include <unistd.h> // Unix标准头文件，提供fork、getpid等系统调用
#include <signal.h> // 信号处理库，用于处理SIGINT和SIGTERM
#include <fcntl.h>  // 文件控制，提供open函数和O_RDWR等标志

#include <filesystem> // 文件系统库，用于创建目录
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sstream>  // 字符串流，用于格式化
#include <sys/stat.h>  // 添加这个头文件

#include "CConfig.h" // 你的配置管理类

bool bExit = false;

// 全局日志记录器
std::shared_ptr<spdlog::logger> g_logger;

// 信号处理函数
void signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        if (g_logger)
        {
            g_logger->warn("收到 SIGINT 信号 (Ctrl+C)，准备退出...");
        }
        std::cout << "\n收到 SIGINT 信号，程序即将退出..." << std::endl;
    }
    else if (signum == SIGTERM)
    {
        if (g_logger)
        {
            g_logger->warn("收到 SIGTERM 信号 (kill命令)，准备退出...");
        }
        std::cout << "\n收到 SIGTERM 信号，程序即将退出..." << std::endl;
    }
    
    bExit = true;


}

// 辅助函数：日志级别字符串转换为spdlog级别
spdlog::level::level_enum stringToLevel(const std::string &level_str)
{
    if (level_str == "trace")
        return spdlog::level::trace;
    if (level_str == "debug")
        return spdlog::level::debug;
    if (level_str == "info")
        return spdlog::level::info;
    if (level_str == "warn")
        return spdlog::level::warn;
    if (level_str == "error")
        return spdlog::level::err;
    if (level_str == "critical")
        return spdlog::level::critical;
    if (level_str == "off")
        return spdlog::level::off;
    return spdlog::level::info; // 默认级别
}

// 辅助函数：获取当前线程ID的字符串表示
std::string get_thread_id_str()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

// 辅助函数：获取文件大小
size_t get_file_size(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : 0;
}

// 测试文件滚动
void test_log_rotation() {
    std::cout << "\n=== 测试文件滚动 ===" << std::endl;
    
    // 记录一些大日志，触发滚动
    for (int i = 0; i < 500; ++i) {
        g_logger->info("滚动测试 {} - 这是一条比较长的日志消息，用于测试文件滚动功能，确保文件能够正常滚动。", i);
        
        // 每写入50条日志，检查一次文件状态
        if ((i + 1) % 50 == 0) {
            g_logger->flush();
            
            // 检查主日志文件大小
            size_t size = get_file_size("logs/log.txt");
            std::cout << "已写入 " << (i + 1) << " 条日志，主文件大小: " 
                      << size << " 字节 (" << size / 1024.0 << " KB)" << std::endl;
            
            // 检查备份文件
            for (int j = 1; j <= 3; ++j) {
                std::string backup_file = "logs/log.txt." + std::to_string(j);
                size = get_file_size(backup_file);
                if (size > 0) {
                    std::cout << "备份文件 " << backup_file << " 大小: " 
                              << size << " 字节" << std::endl;
                }
            }
        }
    }
}

// 初始化日志系统函数
bool initLogging(CConfig &config)
{
    try
    {
        // 1. 获取日志配置参数
        bool log_console = config.GetBoolDefault("log_console", true);
        std::string level_str = config.GetStringDefault("level", "info");
        std::string pattern = config.GetStringDefault("pattern",
                                                      "[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        std::string filename = config.GetStringDefault("filename", "logs/app.log");
        bool immediate_flush = config.GetBoolDefault("immediate_flush", true);
        int max_size = config.GetIntDefault("max_size", 1); 
        int max_files = config.GetIntDefault("max_files", 3);

        // 添加调试信息
        std::cout << "当前工作目录: " << std::filesystem::current_path() << std::endl;
        std::cout << "日志文件名: " << filename << std::endl;

        // 2. 确保日志目录存在
        std::filesystem::path file_path(filename);
        std::cout << "日志文件完整路径: " << std::filesystem::absolute(file_path) << std::endl;
       
        if (file_path.has_parent_path()) {
            std::cout << "创建日志目录: " << file_path.parent_path() << std::endl;
            bool created = std::filesystem::create_directories(file_path.parent_path());
            std::cout << "目录创建" << (created ? "成功" : "失败或已存在") << std::endl;
        }

        // 3. 创建sinks
        std::vector<spdlog::sink_ptr> sinks;

        // 控制台sink
        if (log_console)
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern(pattern);
            sinks.push_back(console_sink);
        }

        // 创建按大小滚动的日志文件
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            filename, max_size * 1024, max_files);
        file_sink->set_pattern(pattern);
        sinks.push_back(file_sink);

        std::cout << "日志文件: " << filename
                  << " (最大 " << max_size << " KB, 保留 " << max_files << " 个文件)" << std::endl;

        // 4. 创建logger
        g_logger = std::make_shared<spdlog::logger>("multi_thread_logger",
                                                    sinks.begin(), sinks.end());

        // 5. 设置日志级别
        g_logger->set_level(stringToLevel(level_str));

        // 6. 设置立即刷新
        if (immediate_flush)
        {
            g_logger->flush_on(stringToLevel(level_str));
        }

        // 7. 设置为全局默认日志器（可选）
        spdlog::set_default_logger(g_logger);

        // 记录初始化成功的日志
        g_logger->info("日志系统初始化成功!");
        g_logger->info("日志级别: {}", level_str);
        g_logger->info("日志文件: {}", filename);
        g_logger->info("最大文件大小: {} MB", max_size);
        g_logger->info("最大文件数: {}", max_files);

        return true;
    }
    catch (const spdlog::spdlog_ex &e)
    {
        std::cerr << "spdlog异常: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "标准异常: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "未知异常" << std::endl;
        return false;
    }
}

// 1、 转换为守护进程的函数
void becomeDaemon()
{
    // 保存当前工作目录，确保日志文件路径正确
    std::string original_cwd = std::filesystem::current_path().string();
    
    g_logger->info("开始转换为守护进程...");
    g_logger->info("原始工作目录: {}", original_cwd);

    pid_t pid = fork(); // 1. 创建子进程
    if (pid < 0)        // pid < 0: 创建失败
    {
        std::cout << "创建子进程失败！" << std::endl,
        exit(EXIT_FAILURE);
    }
    if (pid > 0) // pid > 0: 当前是父进程，pid是子进程ID
    {
        std::cout << "父进程退出，子进程继续运行（PID: " << pid << "）" << std::endl,
        exit(EXIT_SUCCESS); // 2. 父进程退出，让子进程继续运行
    }

    setsid(); // 3. 创建新的会话，使进程脱离终端控制，在新会话中成为领导进程
    std::cout << "新会话创建成功，进程已脱离终端控制。" << std::endl;
    // // 4. 关闭标准输入输出（可选，这里保持打开以便观察）
    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
        // 重定向标准输入输出
    int null_fd = open("/dev/null", O_RDWR);
    if (null_fd != -1)
    {
        dup2(null_fd, STDIN_FILENO);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
        close(null_fd);
    }
    std::cout << "标准输入输出已关闭。" << std::endl; // 不会显示

    // 5. 保持在原工作目录，而不是切换到/tmp
    // 这样可以确保相对路径的日志文件能够正确访问
    // chdir("/tmp");  // 注释掉：避免影响日志文件路径
    chdir(original_cwd.c_str());  // 切换回原工作目录

    g_logger->info("已转换为守护进程运行（PID: {}）", getpid());
    g_logger->info("工作目录: {}", std::filesystem::current_path().string());
}

// 2、线程要执行的任务
void threadTask(int id)
{
    // 获取当前线程ID的字符串
    std::string thread_id_str = get_thread_id_str();

    // 记录线程开始（只在日志中记录，不在控制台输出）
    //std::this_thread::get_id() 无法执行，一直出错，所有换成使用字符串转化函数
    g_logger->info("线程 {} 开始执行 (PID: {}, TID: {})",
                   id, getpid(), thread_id_str);

    int loop_count = 0;
    while(!bExit)
    {
        g_logger->info("线程 {} 正在运行执行第 {} 次任务 (PID: {}, TID: {})",
                   id, ++loop_count, getpid(), thread_id_str);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    g_logger->info("线程 {} 正在运行执行完毕", id);    

    // 打印线程执行信息
    std::cout << "线程 " << id << " 执行完毕 (PID: " << getpid() << ", TID: " << thread_id_str << ")" << std::endl;
}

int main()
{
    // 步骤1：读取配置
    // 获取配置单例的引用（auto& 自动推导类型）
    auto &config = CConfig::GetInstance();

    // 使用绝对路径（相对于可执行文件）
    std::string configPath = "../config/subproject2.yaml";

    // 加载配置文件
    if (!config.Load(configPath))
    {
        // 如果加载失败，打印错误信息
        std::cerr << "警告: " << config.GetLastError() << std::endl;
        std::cerr << "将使用默认配置运行" << std::endl;
    }

    // 初始化日志系统
    std::cout << "正在初始化日志系统..." << std::endl;
    if (!initLogging(config))
    {
        std::cerr << "日志系统初始化失败，程序退出" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "日志系统初始化成功" << std::endl;

    // 直接获取配置值（使用默认值保证安全）
    int threadCount = config.GetIntDefault("thread_count", 2);     // 默认2个线程
    bool daemonMode = config.GetBoolDefault("daemon_mode", false); // 默认前台运行

    std::cout << "配置信息:" << std::endl;
    std::cout << "  - 线程数: " << threadCount << std::endl;
    std::cout << "  - 运行模式: " << (daemonMode ? "守护进程" : "前台进程") << std::endl;

    // 测试文件滚动
    // test_log_rotation();

    // 注册信号处理器
    signal(SIGINT, signalHandler);   // 处理 Ctrl+C
    signal(SIGTERM, signalHandler);  // 处理 kill 命令
    g_logger->info("信号处理器已注册 (SIGINT, SIGTERM)");

    // 在日志中记录关键系统事件
    g_logger->info("========== 应用程序启动 ==========");
    g_logger->info("程序启动 (PID: {})", getpid());
    g_logger->info("线程数: {}", threadCount);
    g_logger->info("运行模式: {}", daemonMode ? "守护进程" : "前台进程");

    // 步骤2：判断是否转为守护进程
    if (daemonMode)
    {
        std::cout << "正在转换为守护进程..." << std::endl;
        becomeDaemon();
    }

    // 步骤3：创建并运行线程
    std::cout << "开始创建 " << threadCount << " 个线程..." << std::endl;

    std::vector<std::thread> threads; // 创建线程容器
    threads.reserve(threadCount);     // 预分配空间，提高效率

    // 循环创建指定数量的线程
    for (int i = 0; i < threadCount; ++i)
    {
        // std::thread(threadTask, i) 创建新线程
        // threadTask: 线程要执行的函数
        // i: 传递给threadTask的参数
        // push_back: 将线程对象添加到vector中
        threads.push_back(std::thread(threadTask, i));
    }

    // 步骤4：等待所有线程完成
    for (auto &thread : threads)
    {
        thread.join();
    }
    // 记录和显示最终状态
    g_logger->info("所有线程执行完毕");
    g_logger->info("========== 应用程序结束 ==========");

    std::cout << "\n所有线程执行完毕，程序即将退出。" << std::endl;

    // std::this_thread::sleep_for(std::chrono::seconds(200)); // 给时间观察输出

    // 如果守护进程模式，持续运行一段时间以便观察
    // if (daemonMode)
    // {
    //     std::cout << "守护进程将持续运行（观察日志文件查看状态）..." << std::endl;
    //     for (int i = 0; i < 20; ++i)
    //     {
    //         std::this_thread::sleep_for(std::chrono::seconds(10));
    //         g_logger->info("守护进程运行中... {}0秒已过", i + 1);
    //     }
    // }
    // else
    // {
    //     // 前台模式，短暂等待后退出
    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    // }

    // 确保所有日志写入文件
    g_logger->flush();
    std::cout << "程序正常退出" << std::endl;

    // 清理日志系统
    spdlog::shutdown();

    return 0;
}