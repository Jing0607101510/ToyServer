/*
@Author: Jing
@Date: 2020.09.18
@Desc: 连接池
*/

#include "ConnectionPool.h"

ConnectionPool& ConnectionPool::getInstance(){
    if(pool == nullptr){
        std::lock_guard<std::mutex> guard(mtx);
        if(pool == nullptr){
            pool = new ConnectionPool();
        }
    }
    return *pool;
}


// 默认配置
ConnectionPool::ConnectionPool(){
    max_num_conn = 20;
    min_num_conn = 2;
    cur_num_conn = 0;
    busy_num_conn = 0;

    user = "user";
    passwd = "user";
    url = "127.0.0.1";
    db_name = "users";
    port = 3306;
}


void  ConnectionPool::init(std::string url, int port, std::string db_name, std::string user, std::string passwd, 
                  int min_num_conn, int max_num_conn){


}

void ConnectionPool::manage_connections(); // 管理线程
void ConnectionPool::destroy_pool(); // 销毁连接池
void ConnectionPool::expand_pool(int size); // 扩大连接池
void ConnectionPool::reduce_pool(int size); // 缩小连接池
ConnectionPool::~ConnectionPool();

MySQL* ConnectionPool::getConnection();
        // 归还连接
void ConnectionPool::retConnection(MySQL* conn);
