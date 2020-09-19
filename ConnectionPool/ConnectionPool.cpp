/*
@Author: Jing
@Date: 2020.09.18
@Desc: 连接池
*/

#include <stdlib.h>
#include <iostream>

#include "ConnectionPool.h"
#include "../Log/Logger.h" // 错误时输出日志

ConnectionPool* ConnectionPool::pool = nullptr;
std::mutex ConnectionPool::pool_mtx;

ConnectionPool& ConnectionPool::getInstance(){
    if(pool == nullptr){
        std::lock_guard<std::mutex> guard(pool_mtx);
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
    url = "localhost";
    db_name = "web";
    db_port = 3306;

    running = true;
}

// 初始化参数，创建连接池
void  ConnectionPool::init(std::string url, int port, std::string db_name, std::string user, std::string passwd, 
                  int min_num_conn, int max_num_conn){
    this->url = url;
    this->db_port = port;
    this->db_name = db_name;
    this->user = user;
    this->passwd = passwd;
    this->min_num_conn = min_num_conn;
    this->max_num_conn = max_num_conn;

    // 初始化 连接池 , 还没有多线程，不用加锁互斥
    for(int i = 0; i < min_num_conn; i++){
        MYSQL* conn = NULL; // 一定要初始化为 NULL
        conn = mysql_init(conn);

        if(conn == NULL){
            LOG_ERROR("MySQL Init Error."); // LOGGER单例模式，不需要单独定义初始化
            exit(1); // 不能接受的错误
        }

        conn = mysql_real_connect(conn, url.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), port, NULL, 0);
        if(conn == NULL){
            std::cout << "ERROR" << std::endl;
            LOG_ERROR("MySQL Connection Error.");
            exit(1);
        }

        conns.push_back(conn);
        cur_num_conn++; // 当前的连接数增加1
    }
    // 初始化信号量
    sem = Sem(cur_num_conn);

    // 启动管理线程
    sp_thread.reset(new std::thread(&ConnectionPool::manage_pool, this));
}

// 管理线程
void ConnectionPool::manage_pool(){ 
    // 使用到 running成员变量
}

// 销毁连接池, 释放所有的连接。正在被使用的连接怎么处理？
// 非运行时才能够销毁
void ConnectionPool::destroy_pool(){ 
    // 以下只是对空闲的连接销毁。
    if(!running){
        std::lock_guard<std::mutex> guard(conn_mtx);
        while(conns.size()){
            MYSQL* conn = conns.back();
            conns.pop_back();
            mysql_close(conn);
            cur_num_conn--;
        }
    }   
}


// 扩大连接池
void ConnectionPool::expand_pool(int size){ 

}

// 缩小连接池
void ConnectionPool::reduce_pool(int size){ 

}

ConnectionPool::~ConnectionPool(){
    running = false;
    sp_thread->join(); // 等待管理线程结束
    destory_pool();
}


MYSQL* ConnectionPool::getConnection(){
    return nullptr;
}


// 归还连接
void ConnectionPool::retConnection(MYSQL* conn){
    // 判断conn是否为NULL
    if(conn == nullptr)
        return;

    std::lock_guard<std::mutex> guard(conn_mtx);
    conns.push_back(conn);
    cur_num_conn++;
    sem.post(); // post 和 wait 是原子操作
}



bool ConnectionPool::stop(){

}







ConnectionRAII::ConnectionRAII(MYSQL** conn, ConnectionPool* pool){

}

ConnectionRAII::~ConnectionRAII(){ // 析构时归还

}