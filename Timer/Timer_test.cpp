#include <iostream>
#include "Timer.h"
#include <time.h>
#include <unistd.h>

using namespace std;


void print(int num){
    cout << "bye bye" << num << endl;
}

int main(void){
    TimerQueue tq;
    time_t t = time(NULL);
    shared_ptr<Timer> t1(new Timer(t+1, bind(print, 1)));
    shared_ptr<Timer> t2(new Timer(t+2, bind(print, 2)));
    shared_ptr<Timer> t3(new Timer(t+3, bind(print, 3)));
    
    tq.addTimer(t1);
    tq.addTimer(t2);
    tq.addTimer(t3);

    cout << t1->isDeleted() << endl
         << t1->getExpiredTime() << endl
         << t1->isExpired(time(NULL)) << endl;
    // timer.setDeleted();
    cout << t1->isDeleted() << endl;   

    sleep(1);
    tq.handleExpiredTimers();

    tq.delTimer(t3);  
}