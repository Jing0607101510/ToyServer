/*
@Author: Jing
@Date: 2020.9.23
@Desc: 服务器类，总体
*/

#ifndef __SERVER_H__
#define __SERVER_H__

#include <memory>

#include "../EventLoop/EventLoop.h"

class Server{
    public:
        Server();
        ~Server();
        void start();
    private:
        int listen_fd;
        EventLoop* loop;
};


#endif