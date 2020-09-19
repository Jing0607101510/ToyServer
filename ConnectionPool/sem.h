/*
@Author: Jing
@Date: 2020.09.19
@Desc: 对semaphore的封装
*/

#ifndef __SEM_H__
#define __SEM_H__

#include <semaphore.h>


class Sem{
    public:
        Sem();
        Sem(int value);
        ~Sem();
        bool wait();
        bool post();
    private:
        sem_t m_sem;
};  


#endif