#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Poller.h"
#include "../Utils/Utils.h"
#include "../Log/Logger.h"

using namespace std;

void func(){
    cout << "进入函数" << endl;
}

vector<shared_ptr<Channel>> channels;

void readHandler(int fd, Poller& poller){
    while(true){
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int ret = accept(fd, (struct sockaddr*)(&addr), &addr_len);
        if(ret < 0) break;
        shared_ptr<Channel> channel(new Channel(ret)); // 被析构
        channels.push_back(channel);
        channel->setEvents(EPOLLIN | EPOLLET);
        channel->setReadHandler(func);
        poller.addChannel(channel);
        cout << "sp count " << channel.use_count() << endl;
    }
}

int main(void){
    // thread t;
    // t.join();
    // cout << "没有错" << endl;
    cout << "开始" << endl;
    Logger::get_instance().init("log", false);
    // cout << getpid() << endl;
    daemon_run();
    // cout << getpid() << endl;
    // cout << "正常" << endl;
    Poller poller;
    int server_fd = socket_bind_listen(9999);
    cout << server_fd << endl;
    cout << set_sock_non_blocking(server_fd) << endl;
    cout << set_sock_nodelay(server_fd) << endl;
    cout << set_sock_nolinger(server_fd) << endl;
    cout << set_sock_reuse_port(server_fd) << endl;
    register_sigaction(SIGCHLD, SIG_IGN, false);
    cout << "Hello World!" << endl;
    
    vector<shared_ptr<Channel>> activeChannels;
    Channel * p = new Channel(server_fd);
    cout << p << endl;
    shared_ptr<Channel> sp_server(p);
    if(sp_server){
        cout << "可以的" << endl;
    }
    shared_ptr<Channel> sp2 = p->getSelf();
    cout << "运行不到这里" << endl;
    sp_server->setEvents(EPOLLIN | EPOLLET);
    sp_server->setReadHandler(bind(readHandler, server_fd, poller));
    poller.addChannel(sp_server);
    cout << "sp 计数 " << sp_server.use_count() << endl;
    while(true){
        poller.poll(activeChannels);
        for(auto sp : activeChannels){
            sp->handleEvents();
        }
    }

    return 0;
}