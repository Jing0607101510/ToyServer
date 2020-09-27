#include <iostream>
#include "Channel.h"
#include "../HttpData/HttpData.h"
#include <functional>
#include <memory>
#include <sys/epoll.h>

using namespace std;

void func(string str){
    cout << str << endl;
}


int main(void){
    int fd = 100;
    Channel ch(fd);
    cout << ch.getFd() << endl;
    ch.setFd(200);
    cout << ch.getFd() << endl;

    ch.setReadHandler(bind(func, "read"));
    ch.setWriteHandler(bind(func, "write"));
    ch.setErrorHandler(bind(func, "error"));
    ch.setConnHandler(bind(func, "conn"));

    std::shared_ptr<HttpData> hd = std::make_shared<HttpData>();
    ch.setHolder(hd);
    ch.getHolder();

    ch.setEvents(EPOLLIN);
    cout << EPOLLIN << " " << ch.getEvents() << endl;

    ch.setRevents(EPOLLOUT | EPOLLIN);
    cout << EPOLLOUT << " " << ch.getRevents() << endl;

    ch.setLastEvents(EPOLLERR);
    cout << EPOLLERR << " " << ch.getLastEvents() << endl;

    ch.handleEvents();
}