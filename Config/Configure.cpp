/*
@Author: Jing
@Date: 2020.09.18
@Desc: 配置类，单例模式
*/

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#include "Configure.h"


Config* Config::config = nullptr;

void Config::parse_args(int argc, char* argv[]){ // 设置参数
    int opt = -1;
    int option_index = -1; // 是long_options的index
    char* opt_str = "abln:p:t:";

    static struct option long_options[] = {
        {"url", required_argument, NULL, 'r'},
        {"db_port", required_argument, NULL, 'd'},
        {"db_user", required_argument, NULL, 'u'},
        {"db_passwd", required_argument, NULL, 'w'},
        {"min_num_conn", required_argument, NULL, 'i'},
        {"max_num_conn", required_argument, NULL, 'x'}
    };

    while((opt = getopt_long(argc, argv, opt_str, long_options, &option_index)) != -1){
        switch(opt){
            case 'a' : 
                log_mode = 1;
                break;
            case 'b' :
                run_backend = true;
                break;
            case 'd' :
                db_port = atoi(optarg);
                break;
            case 'i' : 
                min_num_conn = atoi(optarg);
                break;
            case 'l' :
                enable_logging = true;
                break;
            case 'n' :
                log_file_name = optarg;
                break;
            case 'p' :
                server_port = atoi(optarg);
                break;
            case 'r' :
                db_url = optarg;
                break;
            case 't' :
                num_thread = atoi(optarg);
                break;
            case 'u' :
                db_user = optarg;
                break;
            case 'w' :
                db_passwd = optarg;
                break;
            case 'x' :
                max_num_conn = atoi(optarg);
                break;
            default:
                break;

        }
    }
}

// 只读，所以不用双检查锁
Config& Config::getInstance(){
    if(config == nullptr){
        config = new Config();
    }
    return *config;
}

Config::Config(){ // 默认方式初始化参数
    server_port = 9999;
    num_thread = 8;
    run_backend = false;

    enable_logging = false;
    log_file_name = "logs/log";
    log_mode = 0; // 同步

    db_url = "127.0.0.1";
    db_port = 3306;
    db_user = "user";
    db_passwd = "user";

    min_num_conn = 4;
    max_num_conn = 16;
}
        