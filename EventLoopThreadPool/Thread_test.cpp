#include <iostream>
#include "EventLoopThreadPool.h"
#include "../EventLoop/EventLoop.h"
#include "../Utils/Utils.h"
#include "../Config/Configure.h"
#include "../Log/Logger.h"

using namespace std;

int main(int argc, char* argv[]){
    Config config;
    cout << "Ok" << endl;
    config.parse_args(argc, argv);
    cout << "Ok2" << endl;
    start_log(config);
    LOG_INFO("Testing");
    cout << "Ok3" << endl;
    EventLoop loop;
    cout << "Ok4" << endl;
    EventLoopThreadPool pool(&loop, 5);
    pool.start();
    cout << "finish" << endl;
    EventLoop* loop1 = pool.getNextLoop();
    cout << "loop1" << loop1 << endl;
    EventLoop* loop2 = pool.getNextLoop();
    cout << "loop2" << loop2 << endl;
    EventLoop* loop3 = pool.getNextLoop();
    cout << "loop3" << loop3 << endl;
    EventLoop* loop4 = pool.getNextLoop();
    cout << "loop3" << loop4 << endl;
    EventLoop* loop5 = pool.getNextLoop();
    cout << "loop3" << loop5 << endl;
    EventLoop* loop6 = pool.getNextLoop();
    cout << "loop3" << loop6 << endl;
    cout << "finish2" << endl;
}