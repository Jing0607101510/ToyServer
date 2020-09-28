/*
@Author: Jing
@Date: 2020.09.23
@Desc: 工具函数定义
*/

#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/tcp.h>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <string.h>

#include "Utils.h"
#include "../Log/Logger.h"
#include "../Config/Configure.h"

// 创建socket，绑定地址，调用listen函数
int socket_bind_listen(int port){
    if(port < 0 || port > 65535) return -1;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) return -1;

    // 设置端口复用
    if(set_sock_reuse_port(fd) == false)
        return -1;

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = -1;
    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1) {
        close(fd);
        return -1;
    }

    ret = listen(fd, 2048);
    if(ret == -1) {
        close(fd);
        return -1;
    }

    return fd;
}

// 设置fd为非阻塞
bool set_sock_non_blocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1) return false;
    flag = flag | O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flag);
    if(ret == -1) return false;
    else return true;
}

// 设置fd的Nagle算法关闭
bool set_sock_nodelay(int fd){
    int enable = 1;
    int ret = -1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
    if(ret == -1) return false;
    else return true;
}

// 设置使用优雅关闭
bool set_sock_nolinger(int fd){
    struct linger _linger;
    _linger.l_onoff = 1;
    _linger.l_linger = 30;
    int ret = -1;
    ret = setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&_linger, sizeof(_linger));
    if(ret == -1) return false;
    else return true;
}


// 设置端口重用
bool set_sock_reuse_port(int fd){
    int optval = 1;
    int ret = -1;
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if(ret == -1) return false;
    else return true;
}


// 注册信号处理函数
void register_sigaction(int sig, SA_Handler handler, bool restart){
    struct sigaction sa;
    bzero(&sa, sizeof(sa)); // 默认会屏蔽正在处理的信号
    sa.sa_handler = handler;
    if(restart)
        sa.sa_flags |= SA_RESTART;
    assert(sigaction(sig, &sa, nullptr) != -1);
}


// 设置以守护进程方式运行
void daemon_run(){
    // 创建子进程
    pid_t pid;
    signal(SIGCHLD, SIG_IGN);

    pid = fork();
    if(pid < 0){
        LOG_ERROR("Fork Error.");
        perror("Fork Error.");
        exit(1);
    }
    else if(pid > 0){
        exit(0); // 退出父进程
    }

    setsid();

    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if(fd != -1){
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if(fd > 2){
        close(fd);
    }

    LOG_INFO("Server Run Daemon.");
}

// 初始化日志
void start_log(Config config){
    char log_file_name[512];
    strcpy(log_file_name, config.log_file_name.c_str());
    Logger::get_instance().init(log_file_name, config.run_backend, config.log_mode);
}


pid_t gettid(){
    return static_cast<pid_t>(::syscall(SYS_gettid));
}


bool is_dir_exists(char* dirname){
    if(access(dirname, F_OK) < 0){
        return false;
    }
    else return true;
}

bool create_dir(char* dirname){
    char dir_name[512];
    strcpy(dir_name, dirname);
    int len = strlen(dir_name);
    if(dir_name[len-1] != '/'){
        strcat(dir_name, "/");
        len++;
    }

    for(int i = 0; i < len; i++){
        if(dir_name[i] == '/'){
            dir_name[i] = '\0';
            if(!is_dir_exists(dir_name)){
                if(mkdir(dir_name, 0777) < 0)
                    return false;
            }
            dir_name[i] = '/';
        }
    }
}