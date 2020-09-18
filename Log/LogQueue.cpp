#include "LogQueue.h"

int LogQueue::size(){
    return m_queue.size();
}

// 取出链头元素 和 弹出链头元素 在同一个加锁下进行
bool LogQueue::pop(string& log_msg){
    std::lock_guard<std::mutex> guard(m_mtx);
    if(m_queue.size() > 0){
        m_queue.pop_front();
        return true;
    }
    else return false;
}

void LogQueue::push(char* log_msg){
    std::lock_guard<std::mutex> guard(m_mtx);
    m_queue.emplace_back(log_msg); // 直接构造，减少一次的移动构造或者拷贝构造
}
