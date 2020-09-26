# 成员变量

- 当前loop的状态，是否loop 状态
- 因为在loop中进行IO多路复用，可以就IO多路复用封装为一个类poller，loop持有poller的一个实例。
- wakeupFd用于线程之间的通信，向wakeupFD发送信号，能够使epoll_wait返回。让其在loop中处理某些操作。
- quit：是否退出loop  状态
- eventHandling是否正在处理事件 状态
- callingPendingFunctors:是否正在调用Pendingfunc 状态
- 这个loop对应的thread id
- Channel是什么？



# 成员函数







使用TLS，关键字__thread实现每个线程至多一个loop

