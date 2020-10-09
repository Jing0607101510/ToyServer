/*
@Author: Jing
@Date: 2020.09.26
@Desc: 定时器设计
*/

#include "Timer.h"

Timer::Timer(int timeout, Callback&& cb_func):
            m_cb_func(cb_func),
            m_deleted(false)
{
    time_t now = time(nullptr);
    m_expired_time = now + timeout;
}

Timer::~Timer(){
    if(m_cb_func){
        m_cb_func();
        m_cb_func = nullptr;
    }        
}


bool Timer::isExpired(time_t now){
    if(m_expired_time <= now) return true;
    else return false;
}


bool Timer::isDeleted(){
    return m_deleted;
}


void Timer::setDeleted(){
    m_cb_func = nullptr;
    m_deleted = true;
}


time_t Timer::getExpiredTime(){
    return m_expired_time;
}



void TimerQueue::addTimer(SP_Timer sp_timer){
    if(sp_timer)
        m_timer_queue.push(sp_timer);
}

void TimerQueue::delTimer(SP_Timer sp_timer){
    if(sp_timer)
        sp_timer->setDeleted();
}


void TimerQueue::handleExpiredTimers(){
    time_t now = time(NULL);
    while(m_timer_queue.size()){
        SP_Timer timer = m_timer_queue.top();
        if(timer->isDeleted()){
            m_timer_queue.pop(); // deleted状态的回调函数已经被清空
        }
        else if(timer->isExpired(now)){
            m_timer_queue.pop(); // 因为过期，析构时调用回调函数
        }
        else break;
    }
}