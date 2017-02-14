#ifndef __MULTITHREADING_H__
#define __MULTITHREADING_H__

#include "core/math/type.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadSync {
    public:
        ThreadSync() : sync(false) {}
        void wait();
        void signal();
    private:
        bool sync;
        std::mutex mutex;
        std::condition_variable cond;
};

inline void ThreadSync::wait() {
    std::unique_lock<std::mutex> lck(mutex);
    while(!sync)
        cond.wait(lck);
    sync = false;
}

inline void ThreadSync::signal() {
    std::unique_lock<std::mutex> lck(mutex);
    sync = true;
    cond.notify_all();
}

class ThreadParams {
public:
    void * data;

    bool end;
    positive threadNum, threadCount;
    ThreadSync syncStart;
    ThreadSync syncEnd;
    void (*func_ptr)(void*, positive, positive);
};

class MultiThread {
	public:
        MultiThread(positive threadCount);
        virtual ~MultiThread();
        void execute(void (*func)(void * data, positive threadId, positive threadsCount), void * data);
    private:
        positive threadCount;
        std::thread ** threads;
        ThreadParams * params;
};

#endif
