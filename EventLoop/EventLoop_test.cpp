#include <iostream>
#include "EventLoop.h"
#include "../Utils/Utils.h"
#include <thread>
#include <unistd.h>
#include "../Config/Configure.h"
using namespace std;

void func(){
    cout << "进入func" << endl;
}

void func2(EventLoop& loop){
    sleep(5);
    loop.wakeup();
}

int main(void){
    Config config;
    start_log(config);
    EventLoop loop;
    cout << "OK1" << endl;
    loop.assertInLoopThread();
    cout << "OK2" << endl;
    cout << loop.isInLoopThread() << endl;
    cout << "OK3" << endl;
    loop.queueInLoop(func);
    cout << "OK4" << endl;
    thread t(func2, std::ref(loop));
    cout << "OK5" << endl;
    loop.loop(); // 十秒 ,由于loop是死循环，所以后面得不到执行。
    cout << "OK6" << endl;
    
    t.join();
    sleep(10);
}
