/*
@Author: Jing
@Date: 2020.09.28
@Desc: 封装的event线程
*/

#include <unistd.h>
#include <sys/types.h>
#include <syscall.h>
#include <assert.h>
#include <functional>

#include "EventLoopThread.h"
#include "../Utils/Utils.h"


namespace CurrentThread{
    __thread int t_cachedTid = 0;

    void cacheTid(){
        if(t_cachedTid == 0){
            t_cachedTid = gettid();
        }
    }
}


EventLoopThread::EventLoopThread():
    m_loop(nullptr),
    m_running(false),
    m_tid(-1){}



EventLoopThread::~EventLoopThread(){
    stop();
}


EventLoop* EventLoopThread::startLoop(){
    assert(!m_running);
    m_running = true;
    m_sp_thread.reset(new std::thread(std::bind(&EventLoopThread::worker_threadFunc, this))); // 开启线程
    {
        std::unique_lock<std::mutex> guard(m_mtx);
        while(m_loop == nullptr) m_cond_var.wait(guard);
    }
    return m_loop;
}


void EventLoopThread::stop(){
    if(!m_running) return;
    m_running = false;
    if(m_loop){
        m_loop->quit();
        m_sp_thread->join();
    }
}


void EventLoopThread::worker_threadFunc(){
    // 设置tid
    m_tid = CurrentThread::tid();
    // 创建loop（局部）
    EventLoop loop;
    // 由于m_loop被主线程和当前线程读写，所以需要加锁
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        m_loop = &loop;
        m_cond_var.notify_one();
    }
    // 开启循环
    loop.loop();
    m_loop = nullptr;
}