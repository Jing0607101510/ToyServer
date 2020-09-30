/*
@Author: Jing
@Date: 2020.09.26
@Desc: main
*/

#include "Log/Logger.h"
#include "Server/Server.h"
#include "Config/Configure.h"
#include "Utils/Utils.h"
int main(int argc, char* argv[]){
    // 读取配置
    Config config;
    config.parse_args(argc, argv);
    // 系统设置
    setup_server(config);
    // 启动server和主循环
    EventLoop loop;
    Server server(config, &loop);
    server.start();
    loop.loop();

    return 0;
}