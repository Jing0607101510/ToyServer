/*
@Author: Jing
@Date: 2020.9.26
@Desc: 时间循环
*/

#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

class EventLoop{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void quit();
        void assertInLoopThread();
    private:
        int m_wakeup_fd;
        bool m_looping;
    private:
        int create_wakeup_fd();
};

#endif