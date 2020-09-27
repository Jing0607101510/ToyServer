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
    
    // 开启日志
    start_log(config);
    
    // 是否以守护进程方式运行
    if(config.run_backend)
        daemon_run();

    Server server;
    EventLoop loop;
    server.start();
    loop.loop();
    return 0;
}