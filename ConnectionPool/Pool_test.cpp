#include <iostream>
#include <unistd.h>
#include <thread>

#include "ConnectionPool.h"
#include "../Log/Logger.h"

using namespace std;

void func(ConnectionPool* pool, int id){
    // 进入线程
    cout << id << "进入线程" << endl;
    pool->sem.wait();
    cout << id << "退出线程" << endl;
}

void func2(ConnectionPool* pool, int id){
    cout << id << "进入线程" << endl;
    pool->sem.post();
    cout << id << "退出线程" << endl;
}

void func0(ConnectionPool* pool, int id){
    cout << id << "进入线程" << endl;
    for(int i = 0; i < 14; i++)
        pool->sem.post();
}

int main(void){
    Logger::get_instance().init("../Log/logs/", 3L, LOG_MODE_ASYNC);
    ConnectionPool& pool = ConnectionPool::getInstance();
    cout << "OK!" << endl;
    pool.init("localhost", 3306, "web", "user", "user", 2, 20); // 单例模式还需要初始化。

    cout << "OK2" << endl;
    
    // pool.sem.post();
    // pool.sem.post();
    // pool.sem.post();

    pool.sem.wait();
    cout << "剩余的连接" << pool.sem.getvalue() << endl;
    pool.sem.wait();
    cout << "剩余的连接" << pool.sem.getvalue() << endl;

    thread t1(func, &pool, 1);
    thread t2(func, &pool, 2);
    sleep(2);
    thread t3(func, &pool, 3);
    thread t4(func, &pool, 4);
    sleep(2);
    thread t5(func, &pool, 3);
    thread t6(func, &pool, 4);
    sleep(2);
    thread t7(func, &pool, 3);
    thread t8(func, &pool, 4);
    sleep(2);
    thread t9(func, &pool, 3);
    thread t10(func, &pool, 4);
    sleep(2);
    thread t11(func, &pool, 3);
    thread t12(func, &pool, 4);
    
    sleep(6);
    thread t0(func0, &pool, 0);

    pool.sem.wait();
    cout << "跑到这里了" << pool.sem.getvalue() << endl;
    sleep(1);
    // MYSQL* conn1 = pool.getConnection();
    // cout << "OK5" << endl;
    // pool.retConnection(conn1);
    // cout << "OK50" << endl;
    // MYSQL* conn2 = pool.getConnection();
    // cout << "OK6" << endl;
    // MYSQL* conn3 = pool.getConnection();
    // cout << "OK7" << endl;
    // MYSQL* conn4 = pool.getConnection();
    // cout << "OK8" << endl;
    // MYSQL* conn5 = pool.getConnection();
    // cout << "OK9" << endl;
    // MYSQL* conn6 = pool.getConnection();
    // cout << "OK10" << endl;
    return 0;
}