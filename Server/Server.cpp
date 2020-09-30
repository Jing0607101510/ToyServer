/*
@Author: Jing
@Date: 2020.9.23
@Desc: 服务器类，总体
*/

#include <sys/epoll.h>

#include "Server.h"
#include "../Utils/Utils.h"

Server::Server(Config config, EventLoop* loop):
    m_listen_fd(socket_bind_listen(config.server_port)),
    m_listen_channel(new Channel(m_listen_fd)),
    m_loop(loop),
    m_running(false),
    m_thread_pool(new EventLoopThreadPool(loop, config.num_thread))
{   
    if(!set_sock_non_blocking(m_listen_fd)) abort();
    if(!set_sock_reuse_port(m_listen_fd)) abort();

    m_listen_channel->setReadHandler(std::bind(&Server::readHandler, this));
    m_listen_channel->setConnHandler(std::bind(&Server::connHandler, this));
    m_listen_channel->setEvents(EPOLLIN | EPOLLET); // 读事件 和 边缘触发
}

// 应该退出各个loop，设置信号处理函数
Server::~Server(){}


void Server::start(){
    if(m_running) return;
    m_running = true;
    m_thread_pool->start(); // 开启线程池
    m_loop->addToPoller(m_listen_channel);
}

void Server::readHandler(){

}

void Server::connHandler(){
    // TODO
}