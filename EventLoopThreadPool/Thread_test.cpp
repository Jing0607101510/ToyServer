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
    EventLoopThreadPool pool(&loop, 2);
}