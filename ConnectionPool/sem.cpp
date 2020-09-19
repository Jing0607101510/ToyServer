/*
@Author: Jing
@Date: 2020.09.19
@Desc: 对semaphore的封装
*/

#include "sem.h"

Sem::Sem(){
    sem_init(&m_sem, 0, 0);
}

Sem::Sem(int value){
    sem_init(&m_sem, 0, value);
}

Sem::~Sem(){
    sem_destroy(&m_sem);
}

bool Sem::wait(){
    return sem_wait(&m_sem) == 0;
}

bool Sem::post(){
    return sem_post(&m_sem) == 0;
}

int Sem::getvalue(){
    int value;
    sem_getvalue(&m_sem, &value);
    return value;
}