/*
@Author: Jing
@Date: 2020.09.13
@Description: 日志系统, 测试文件
*/
#include <iostream>
#include <unistd.h>
#include "Logger.h"

int main(void){
    Logger::get_instance().init("logs/", 3L, LOG_MODE_ASYNC);

    LOG_INFO("%s %s %d", "I am", " very beautiful!", 100);
    sleep(1);
    LOG_DEBUG("%d %s", 10, "Hello World");
    sleep(1);
    LOG_WARNING("%s", "down");
    sleep(1);
    LOG_ERROR("%s", "发生致命错误");

    Logger::get_instance().stop();

    LOG_ERROR("%s", "测试");

    sleep(5);
}