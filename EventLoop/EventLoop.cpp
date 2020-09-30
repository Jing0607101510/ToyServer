/*
@Author: Jing
@Date: 2020.9.26
@Desc: 事件循环
*/

#include "EventLoop.h"

__thread EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop(){

}

EventLoop::~EventLoop(){

}

void EventLoop::loop(){

}

void EventLoop::quit(){

}

void EventLoop::assertInLoopThread(){

}

int EventLoop::create_wakeup_fd(){
    
}

// 加入到Poller中监听
void addToPoller(std::shared_ptr<Channel> sp_channel, int timeout=0){

}

// 向eventfd中写
void wakeup(){

}
