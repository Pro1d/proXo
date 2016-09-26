#include "Multithreading.h"
#include "../math/type.h"
#include <thread>

void threadHost(ThreadParams * p) {
    while(!p->end) {
        p->syncStart.wait();

        if(p->func_ptr != NULL)
            p->func_ptr(p->data, p->threadNum, p->threadCount);

        p->syncEnd.signal();
    }
}

MultiThread::MultiThread(positive threadCount) :
        threadCount(threadCount),
        threads(new std::thread*[threadCount]),
        params(new ThreadParams[threadCount])
{
    for(positive i = 0; i < threadCount; i++) {
        params[i].threadNum = i;
        params[i].threadCount = threadCount;
        params[i].end = false;
        threads[i] = new std::thread(threadHost, &params[i]);
    }
}

MultiThread::~MultiThread()
{
    for(positive i = 0; i < threadCount; i++) {
        params[i].func_ptr = NULL;
        params[i].data = NULL;
        params[i].end = true;
        params[i].syncStart.signal();
        params[i].syncEnd.wait();
        threads[i]->join();
        delete threads[i];
    }
    delete[] params;
    delete[] threads;
}

void MultiThread::execute(void (*func)(void * data, positive threadId, positive threadsCount), void* data)
{
    for(positive i = 0; i < threadCount; i++)
    {
        params[i].func_ptr = func;
        params[i].data = data;
        params[i].syncStart.signal();
    }

    for(positive i = 0; i < threadCount; i++)
        params[i].syncEnd.wait();
}

