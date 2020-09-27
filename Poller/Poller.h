/*
@Author: Jing
@Date: 2020.09.27
@Desc: epoll的封装
*/

#ifndef __POLLER_H__
#define __POLLER_H__

#include <vector>
#include <sys/epoll.h>

#include "../Channel/Channel.h"

class Poller{
    public:
        Poller(int max_events=4096);
        ~Poller();
        void poll(std::vector<std::shared_ptr<Channel>>& activeChannels);
        void getActiveChannels(int active_num, std::vector<std::shared_ptr<Channel>>& activeChannels);
        void addChannel(std::shared_ptr<Channel> channel);
        void delChannel(std::shared_ptr<Channel> channel);
        void modChannel(std::shared_ptr<Channel> channel);
    private:
        std::vector<epoll_event> m_epoll_events;
        int m_epoll_fd; 
        const int max_events;
};


#endif