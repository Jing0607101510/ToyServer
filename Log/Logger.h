/*
@Author: Jing
@Date: 2020.09.13
@Description: 日志系统, 设计为单例模式 头文件
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <mutex>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <stdarg.h>
#include <memory.h>
#include <condition_variable>
#include <list>
#include <string>
#include <thread>
#include <functional>

using std::min;

enum LOG_LEVEL{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

enum LOG_MODE{
    LOG_MODE_SYNC,
    LOG_MODE_ASYNC,
};

// 单例模式，懒汉模式，
// 为了保证线程安全，
// 1.使用局部静态变量
// 2.使用双检查锁

// 变量和函数分开写

class Logger{
    public:
        // 使用局部静态变量
        static Logger& get_instance(){
            static Logger instance;
            return instance;
        }
        bool init(char* log_file_name, int cnt_split_file, long log_mode=LOG_MODE_SYNC); // 初始化日志系统中的参数
        void append_log(long level, char* format, ...); 
        void stop(); // 暂停日志系统
        bool is_running(){return m_running;}

    private:
        Logger();
        ~Logger();
        void async_write_log();
        Logger(const Logger& logger) = delete;
        Logger& operator=(Logger& logger) = delete;

        

    private:
        char m_dir_name[256]; // 日志目录
        char m_file_name[256]; // 日志文件名
        FILE* m_log_file; // 打开的文件
        
        int m_log_cnt; // 当前所记录的日志条数
        int m_split_cnt; // 划分到第几个文件 
        int m_cnt_split_file; // 每个文件的日志条数, 初始化后不变
        int m_today; // 当前天

        bool m_log_mode; // 日志模式：同步或者异步
        bool m_running; // 处理日志的线程在running为false时，退出。 // 或者是否有开启日志


        // 自定义日志队列类，使用锁来保证线程安全
        std::list<std::string> m_log_queue;
        // 用到的互斥锁
        std::mutex m_mtx; // 用于其他成员变量的互斥访问
        // 异步日志系统 日志队列用到的 条件变量 及 对应的 互斥锁
        std::condition_variable m_cond_var;
        std::mutex m_queue_mtx;
        // 工作线程
        std::shared_ptr<std::thread> m_sp_thread; // 引用为0时，释放堆上的内存
};

#define LOG_DEBUG(format, ...) if(Logger::get_instance().is_running()){Logger::get_instance().append_log(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__);}
#define LOG_INFO(format, ...) if(Logger::get_instance().is_running()){Logger::get_instance().append_log(LOG_LEVEL_INFO, format, ##__VA_ARGS__);}
#define LOG_WARNING(format, ...) if(Logger::get_instance().is_running()){Logger::get_instance().append_log(LOG_LEVEL_WARNING, format, ##__VA_ARGS__);}
#define LOG_ERROR(format, ...) if(Logger::get_instance().is_running()){Logger::get_instance().append_log(LOG_LEVEL_ERROR, format, ##__VA_ARGS__);}

#endif