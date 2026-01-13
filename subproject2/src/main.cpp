// 掌握C++多线程框架程序的开发
// 并可以通过YAML格式配置文件配置成前台或守护进程运行

#include <iostream>   // 输入输出流，用于控制台打印
#include <vector>     // 动态数组容器，存储线程对象
#include <thread>     // C++11线程库，创建和管理线程
#include <chrono>     // 时间库，处理时间间隔
// #include <cstdlib>    // C标准库，提供exit等函数
#include <unistd.h>   // Unix标准头文件，提供fork、getpid等系统调用
// #include <sys/stat.h> // 文件状态头文件，用于chdir等
#include "CConfig.h"  // 你的配置管理类

using namespace std;

// 1、 转换为守护进程的函数
void becomeDaemon()
{
    pid_t pid = fork(); // 1. 创建子进程
    if (pid < 0)        // pid < 0: 创建失败
        cout<<"创建子进程失败！"<<endl,
        exit(EXIT_FAILURE);
    if (pid > 0)            // pid > 0: 当前是父进程，pid是子进程ID
        cout<<"父进程退出，子进程继续运行（PID: "<<pid<<"）"<<endl,
        exit(EXIT_SUCCESS); // 2. 父进程退出，让子进程继续运行

    setsid(); // 3. 创建新的会话，使进程脱离终端控制，在新会话中成为领导进程
    std::cout << "新会话创建成功，进程已脱离终端控制。" << std::endl;
    // // 4. 关闭标准输入输出（可选，这里保持打开以便观察）
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    std::cout << "标准输入输出已关闭。" << std::endl;//不会显示

    // 5. 将工作目录改为根目录（避免占用可卸载的文件系统）
    chdir("/tmp");
    //不会显示
    std::cout << "已转换为守护进程运行（PID: " << getpid() << "）" << std::endl;
}

// 2、线程要执行的任务
void threadTask(int id)
{
    // 模拟工作：让线程睡眠1秒
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 打印线程执行信息
    // getpid() 获取当前进程ID
    // std::this_thread::get_id() 获取当前线程ID
    std::cout << "线程 " << id << " 执行完毕 (PID: " << getpid() << ", TID: " << std::this_thread::get_id() << ")" << std::endl;
}

int main()
{
    // 步骤1：读取配置
    // 获取配置单例的引用（auto& 自动推导类型）
    auto& config = CConfig::GetInstance();

    // 使用绝对路径（相对于可执行文件）
    std::string configPath = "../config/subproject2.yaml";
    
    // 加载配置文件
    if (!config.Load(configPath)) {
        // 如果加载失败，打印错误信息
        std::cerr << "警告: " << config.GetLastError() << std::endl;
        std::cerr << "将使用默认配置运行" << std::endl;
    }
    
    // 直接获取配置值（使用默认值保证安全）
    int threadCount = config.GetIntDefault("thread_count", 2);    // 默认2个线程
    bool daemonMode = config.GetBoolDefault("daemon_mode", false); // 默认前台运行
    
    std::cout << "配置信息:" << std::endl;
    std::cout << "  - 线程数: " << threadCount << std::endl;
    std::cout << "  - 运行模式: " << (daemonMode ? "守护进程" : "前台进程") << std::endl;

    // 步骤2：判断是否转为守护进程
    if (daemonMode)
    {
        std::cout << "正在转换为守护进程..." << std::endl;
        becomeDaemon();
    }

    // 步骤3：创建并运行线程
    std::cout << "开始创建 " << threadCount << " 个线程..." << std::endl;

    std::vector<std::thread> threads; // 创建线程容器
    threads.reserve(threadCount); // 预分配空间，提高效率

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

    std::cout << "所有线程执行完毕，程序即将退出。" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(200)); // 给时间观察输出

    return 0;
}