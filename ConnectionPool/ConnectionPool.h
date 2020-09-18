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


class ConnectionPool{
    public:
        static ConnectionPool& getInstance();
        MySQL* getConnection();
        void retConnection(MySQL* conn);

    private:
        
    public:

    private:
        // 单例模式
        static ConnectionPool* pool;
        static std::mutex pool_mtx;

        // 连接池，使用list实现
        std::list<MySQL*> conns;

        // 管理线程
        std::thread manager
};

class ConnectionRAII{

};

#endif