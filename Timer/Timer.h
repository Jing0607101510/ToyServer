/*
@Author: Jing
@Date: 2020.09.26
@Desc: 定时器设计
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <time.h>
#include <functional>
#include <queue>
#include <memory>

class Timer{
    public:
        typedef std::function<void()> Callback;
        Timer(time_t expired_time, Callback&& cb_func);
        ~Timer();
        bool isExpired(time_t now);
        bool isDeleted();
        void setDeleted();
        time_t getExpiredTime();
    private:
        time_t m_expired_time;
        bool m_deleted;
        Callback m_cb_func;

};


struct TimerCmp{
    bool operator()(const std::shared_ptr<Timer>& t1, const std::shared_ptr<Timer>& t2){
        return t1->getExpiredTime() >= t2->getExpiredTime(); // 小顶堆，使用大于
    }
};

class TimerQueue{
    public:
        typedef std::shared_ptr<Timer> SP_Timer;
        TimerQueue(){}
        ~TimerQueue(){}
        void addTimer(SP_Timer sp_timer);
        void delTimer(SP_Timer sp_timer);
        void handleExpiredTimers();
    private:
        
        std::priority_queue<SP_Timer, std::deque<SP_Timer>, TimerCmp> m_timer_queue;
};


#endif