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