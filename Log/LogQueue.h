/*
@Author: Jing
@Date: 2020.09.16
@Description: 线程安全的日志队列，使用std::list
*/

#ifndef __LOG_QUEUE_H__
#define __LOG_QUEUE_H__

#include <list>
#include <mutex>

class LogQueue{
    public:
        LogQueue(){};
        ~LogQueue(){};
        int size();
        bool pop();
        void push(char* log_msg);
    private:
        std::mutex m_mtx; 
        std::list<string> m_queue; // list是双向链表
};

#endif
