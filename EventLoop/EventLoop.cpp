/*
@Author: Jing
@Date: 2020.9.26
@Desc: 时间循环
*/

#include "EventLoop.h"

__thread EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop();
EventLoop::~EventLoop();
void EventLoop::loop();