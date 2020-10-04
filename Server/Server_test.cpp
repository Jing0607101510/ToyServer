#include <iostream>
#include "Server.h"
#include "../Config/Configure.h"
#include "../EventLoop/EventLoop.h"
#include "../Utils/Utils.h"

using namespace std;

int main(int argc, char* argv[]){
    Config config;
    config.parse_args(argc, argv);
    start_log(config);
    cout << "OK4" << endl;
    EventLoop loop;
    cout << "OK5" << endl;
    Server server(config, &loop);
    cout << "OK1" << endl;
    server.start();
    cout << "OK2" << endl;
    server.readHandler();
    cout << "OK3" << endl;
    return 0;
}