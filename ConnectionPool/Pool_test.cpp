#include <iostream>

#include "ConnectionPool.h"
#include "../Log/Logger.h"

using namespace std;

int main(void){
    Logger::get_instance().init("../Log/logs/", 3L, LOG_MODE_ASYNC);
    ConnectionPool& pool = ConnectionPool::getInstance();
    cout << "OK!" << endl;
    pool.init("localhost", 3306, "web", "user", "user", 2, 20); // 单例模式还需要初始化。
    cout << "OK2" << endl;
    Logger::get_instance().stop();
    return 0;
}