/*
@Author: Jing
@Date: 2020.9.26
@Desc: 事件循环
*/

#include <unistd.h>
#include <sys/epoll.h>
#include <functional>
#include <assert.h>

#include "EventLoop.h"
#include "../Utils/Utils.h"
#include "../Channel/Channel.h"
#include "../Poller/Poller.h"
#include "../Timer/Timer.h"
#include "../Log/Logger.h"
#include "../EventLoopThreadPool/EventLoopThread.h"

__thread EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop():
    m_wakeup_fd(create_wakeup_fd()),
    m_wakeup_channel(new Channel(m_wakeup_fd)),
    m_poller(new Poller()),
    thread_id(CurrentThread::tid()),
    m_looping(false),
    m_doing_pending(false),
    m_timer_queue(new TimerQueue())
{
    // 设置 t_loopInThisThread;
    if(t_loopInThisThread == nullptr){
        t_loopInThisThread = this;
    }  
    else{

    }
    // 绑定wakeupfd的读处理函数，监听的事件
    m_wakeup_channel->setEvents(EPOLLIN | EPOLLET);
    m_wakeup_channel->setReadHandler(std::bind(&EventLoop::readHandler, this));
    // 将channel加入到poller中
    m_poller->addChannel(m_wakeup_channel);
}

EventLoop::~EventLoop(){
    close(m_wakeup_fd);
    // TODO 其他操作？
}

void EventLoop::loop(){
    sleep(5);
}

void EventLoop::quit(){
    if(!m_looping) return;
    m_looping = false;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::assertInLoopThread(){
    assert(isInLoopThread());
}

bool EventLoop::isInLoopThread(){
    return thread_id == CurrentThread::tid();
}

void EventLoop::queueInLoop(Functor&& func){

}


// 加入到Poller中监听
void EventLoop::addToPoller(std::shared_ptr<Channel> sp_channel, int timeout){

}

// 向eventfd中写
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t ret = write(m_wakeup_fd, &one, sizeof(one));
    if (ret != sizeof(one)) {
        LOG_INFO("EventLoop::wakeup() writes %d bytes instead of 8", ret);
    }
}


void EventLoop::readHandler(){

}


void EventLoop::doPendingFunctors(){

}


void EventLoop::handleExpired(){

}
