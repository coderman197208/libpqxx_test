#pragma once

#include <string>
#include <iostream>  // 添加这一行，用于 std::cerr
#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

// 配置管理类————负责读取配置文件
class CConfig {
public:
    
    // 1、获取单例实例
    static CConfig* GetInstance() 
    {	
        // 第一次检查
        if(m_instance == NULL) //如果实例不存在，则创建实例
        {
            // 第二次检查
            if(m_instance == NULL) //防止多个线程同时通过第一次检查后，重复创建实例
            {					
                m_instance = new CConfig();// 创建单例
                static CGarhuishou cl;// 创建静态的CGarhuishou对象
            }
        }

        return m_instance;//如果实例存在，则直接返回实例
    }

    // 2、类中套类，用于释放对象	
    class CGarhuishou 
    {
    public:				
        ~CGarhuishou()
        {
            if (CConfig::m_instance)
            {						
                delete CConfig::m_instance;	//析构函数中删除单例实例			
                CConfig::m_instance = NULL;				
            }
        }
    };

    //3、加载配置文件
    bool Load(const char *pconfName)
    {
        // 直接加载YAML配置文件
        //YAML::Node config = YAML::LoadFile(pconfName);

        try {
            config = YAML::LoadFile(pconfName);
            return true;
        } catch (const YAML::Exception& e) {
            // 【关键修改】在日志系统初始化前，使用 std::cerr 而非 spdlog::error
            // 这打破了与 spdlog 的依赖循环，是更干净的设计。
            //spdlog::error("没有成功加载配置文件: {}", e.what());
            
            std::cerr << "[CConfig Error] 无法加载配置文件 '" << pconfName << "': " << e.what() << std::endl;
            return false;
        }
    }

    //4、获取配置项
    //获取字符串
    std::string GetString(const char *p_itemname)
    {
      
        return config[p_itemname].as<std::string>();

    }
    std::string GetStringDefault(const char* p_itemname, std::string default_value ) {
        if (config[p_itemname]){
            return config[p_itemname].as<std::string>();
        }
        return default_value;
    }

    //获取整型
    int GetInt(const char *p_itemname)
    {
 
        return config[p_itemname].as<int>();

    }

    int GetIntDefault(const char *p_itemname, int default_value)
    {
        if (config[p_itemname]) {
            return config[p_itemname].as<int>();
        }
        return default_value;
    }

    //获取双精度浮点型
    double GetDouble(const char *p_itemname)
    {

        return config[p_itemname].as<double>();

    }
    double GetdoubleDefault(const char *p_itemname, double default_value)
    {
        if (config[p_itemname]) {
            return config[p_itemname].as<double>();
        }
        return default_value;
    }

    //获取布尔型
    bool GetBool(const char *p_itemname)
    {
        return config[p_itemname].as<bool>();
    }
    bool GetBoolDefault(const char *p_itemname, bool default_value)
    {
        if (config[p_itemname]) {
            return config[p_itemname].as<bool>();
        }
        return default_value;
    }
    
private:

    static CConfig *m_instance; //创建静态配置管理类对象

    YAML::Node config;//存储解析后的配置数据,整个类都可以访问这个配置节点
};

CConfig* CConfig::m_instance = NULL;

//如何加载配置文件
//CConfig* p_config = CConfig::GetInstance(); //单例类

//获取配置实例并加载配置文件
        //auto config = CConfig::GetInstance();