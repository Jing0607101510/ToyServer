#include <iostream>
#include <unistd.h>
#include <memory>

#include "HttpConn.h"
#include "../Config/Configure.h"
#include "../EventLoop/EventLoop.h"
#include "../Log/Logger.h"
#include "../Utils/Utils.h"
#include "../Timer/Timer.h"

using namespace std;

void func(){
    cout << "callback" << endl;
}

int main(int argc, char* argv[]){
    Config config;
    config.parse_args(argc, argv);
    setup_server(config);
    EventLoop loop;
    cout << "hello" << endl;
    shared_ptr<HttpConn> conn(new HttpConn(&loop, STDIN_FILENO, "test", config.server_root));
    // cout << "setTimer" << endl;
    // shared_ptr<Timer> timer(new Timer(10, func));
    // conn.setTimer(timer);
    cout << "getName" << endl;
    cout <<conn->getName() << endl;
    
    cout << "newConn" << endl;
    conn->newConn();

    cout << "closeHandler" << endl;
    conn->closeHandler();

    sleep(5);
    cout << "exit" << endl;
}