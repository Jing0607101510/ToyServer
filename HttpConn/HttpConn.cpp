/*
@Author: Jing
@Date: 2020.09.27
@Desc: 封装http数据，及其处理函数
*/

#include <sys/epoll.h>

#include "HttpConn.h"

const int DEFAULT_EXPIRED_TIME = 2000; // ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms

HttpConn::HttpConn(EventLoop* loop, int conn_fd):
    m_loop(loop),
    m_conn_fd(conn_fd),
    m_channel(new Channel(conn_fd))
{
    m_channel->setEvents(EPOLLET | EPOLLIN);
    m_channel->setReadHandler(std::bind(&HttpConn::readHandler, this));
    m_channel->setWriteHandler(std::bind(&HttpConn::writeHandler, this));
    m_channel->setErrorHandler(std::bind(&HttpConn::errorHandler, this));
    m_channel->setConnHandler(std::bind(&HttpConn::connHandler, this));
    m_channel->setCloseHandler(std::bind(&HttpConn::closeHandler, this));
}

HttpConn::~HttpConn(){
    close(m_conn_fd);
    // TODO : 还有其他事情吗
}

void HttpConn::readHandler(){

}

void HttpConn::writeHandler(){

}

void HttpConn::closeHandler(){

}

void HttpConn::errorHandler(){

}

void HttpConn::connHandler(){

}


void HttpConn::setTimer(std::shared_ptr<Timer> timer);



bool HttpConn::read();
bool HttpConn::write();

LINE_STATE HttpConn::parse_line(){

}

HTTP_CODE HttpConn::analyse_request(){

}

HTTP_CODE HttpConn::parse_request_line(){

}

HTTP_CODE HttpConn::parse_headers(){

}

HTTP_CODE HttpConn::parse_content(){

}

HTTP_CODE HttpConn::do_request(){

}


void HttpConn::reset(){
    
}