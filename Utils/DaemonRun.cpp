#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include "DaemonRun.h"
#include "../Log/Logger.h"

// 定义
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