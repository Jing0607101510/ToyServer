/*
@Author: Jing
@Date: 2020.09.27
@Desc: 封装http数据，及其处理函数
*/

#include <sys/epoll.h>

#include "HttpData.h"


HttpData::HttpData(EventLoop* loop, int conn_fd):
    m_loop(loop),
    m_conn_fd(conn_fd),
    m_timer(new Timer()), // 需要指定超时时间和callback
    m_channel(new Channel(conn_fd))
{
    m_channel->setEvents(EPOLLET | EPOLLIN);
    m_channel->setReadHandler(std::bind(&HttpData::readHandler, this));
    m_channel->setWriteHandler(std::bind(&HttpData::writeHandler, this));
    m_channel->setErrorHandler(std::bind(&HttpData::errorHandler, this));
    m_channel->setConnHandler(std::bind(&HttpData::connHandler, this));
}

HttpData::~HttpData(){
    close(m_conn_fd);
    // TODO : 还有其他事情吗
}

void HttpData::readHandler(){

}

void HttpData::writeHandler(){

}

void HttpData::closeHandler(){

}

void HttpData::errorHandler(){

}

void HttpData::connHandler(){

}