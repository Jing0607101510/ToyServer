/*
@Author: Jing
@Date: 2020.10.05
@Desc: 封装http数据，及其处理函数
*/

#ifndef __HTTP_DATA_H__
#define __HTTP_DATA_H__

#include <memory>
#include <string>

#include "../Timer/Timer.h"
#include "../EventLoop/EventLoop.h"
#include "../Channel/Channel.h"

class HttpData{
    public:
        HttpData();
        ~HttpData();

    private:
        EventLoop* m_loop;
        std::shared_ptr<Timer> m_timer;
        std::shared_ptr<Channel> m_channel;
        int m_conn_fd;

        std::string m_in_buf;
        std::string m_out_buf;

    private:
        void readHandler();
        void writeHandler();
        void closeHandler();
        void errorHandler();
        void connHandler();
};


#endif