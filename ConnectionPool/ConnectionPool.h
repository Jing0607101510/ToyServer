/*
@Author: Jing
@Date: 2020.09.18
@Desc: 连接池，单例模式，使用双检查锁
*/

#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__

#include <mutex>
#include <list>
#include <thread>
#include <memory>
#include <string>

class ConnectionPool{
    public:
        static ConnectionPool& getInstance();
        void init(std::string url, int port, std::string db_name, std::string user, std::string passwd, 
                  int min_num_conn, int max_num_conn);
        // 获取连接
        MySQL* getConnection();
        // 归还连接
        void retConnection(MySQL* conn);

    private:
        void manage_connections(); // 管理线程
        void destroy_pool(); // 销毁连接池
        void expand_pool(int size); // 扩大连接池
        void reduce_pool(int size); // 缩小连接池
        ~ConnectionPool();
        ConnectionPool();

    private:
        // 单例模式
        static ConnectionPool* pool;
        static std::mutex pool_mtx;

        // 连接池，使用list实现
        std::list<MySQL*> conns;
        std::mutex conn_mtx; // 连接池链表的互斥

        // 管理线程 用于定时扩缩连接池
        std::shared_ptr<std::thread> sp_thread;

        // 连接池容量相关
        int max_num_conn; // 最大连接数
        int min_num_conn; // 最小连接数
        int cur_num_conn; // 当前建立的连接数
        int busy_num_conn; // 正在被使用的连接数

        // 数据库连接相关
        std::string user;
        std::string passwd;
        std::string url;
        std::string db_name;
        int db_port;

        
};

class ConnectionRAII{
    public:
        ConnectionRAII(MySQL** conn, ConnectionPool* pool);
        ~COnnectionRAII(); // 析构时归还
    private:
        MySQL* conn;
        ConnectionPool* pool;
};

#endif