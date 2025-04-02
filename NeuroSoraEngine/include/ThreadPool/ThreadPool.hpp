#pragma once
#include "NeuroSoraCore.hpp"

class ThreadPool
{
public:
    // // Constructor to creates a thread pool with given
    // number of threads
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    // Enqueue task for execution by the thread pool
    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()> > tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};

static ThreadPool threadPool1;