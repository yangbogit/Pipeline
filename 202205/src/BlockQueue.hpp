#ifndef PIPELINE_BLOCKQUEUE_HPP
#define PIPELINE_BLOCKQUEUE_HPP

#include <queue>
#include <pthread.h>
#include <unistd.h>

/**
 * 阻塞队列：用于两个线程之间的数据传输
 * */

template <typename T>
class BlockQueue
{
private:
    int bqMaxSize;      // 当前队列的最大容量
    std::queue<T> q;    // 队列

    pthread_mutex_t bqMutex;        // 阻塞队列的互斥锁
    pthread_cond_t bqNotFullCond;   // 队列 “非满” 条件变量
    pthread_cond_t bqNotEmptyCond;  // 队列 “非空” 条件变量

public:

    /**
     * 初始化阻塞队列
     * */
    BlockQueue(int maxSize)
    {
        // 队列容量初始化
        bqMaxSize = maxSize;

        // 初始化互斥锁、条件变量
        pthread_mutex_init(&bqMutex, NULL);
        pthread_cond_init(&bqNotFullCond, NULL);
        pthread_cond_init(&bqNotEmptyCond, NULL);
    }

    /**
     * push 一个元素 t
     * 入队列需要等待满足队列 “非满” 条件变量
     * */
    void push(T &t)
    {
        // 尝试获得访问阻塞队列的锁
        pthread_mutex_lock(&bqMutex);

        // 如果队列的容量已经 >= 最大容量, 则当前线程因为队列满而被阻塞，然后开始等待队列非满的信号唤醒当前线程
        // 被唤醒后再次检查，如果仍然 >= 最大容量，继续阻塞当前线程
        // 阻塞的时候自动释放锁，唤醒后自动获得锁
        while (q.size() >= bqMaxSize)
            pthread_cond_wait(&bqNotFullCond, &bqMutex);

        q.push(t);

        // 解锁
        pthread_mutex_unlock(&bqMutex);

        // 唤醒等待队列非空的读取线程
        pthread_cond_signal(&bqNotEmptyCond);
    }

    /**
     * pop 一个元素, 并使用t返回这个元素
     * 出队列需要满足队列“非空”条件变量
     * */
    void pop(T &t)
    {
        // 尝试获得访问阻塞队列的锁
        pthread_mutex_lock(&bqMutex);

        // 如果队列为空, 则当前线程因为队列空阻塞，然后开始等待队列非空的信号唤醒当前线程
        // 被唤醒后再次检查，如果仍然为空，则继续阻塞当前线程
        // 阻塞的时候自动释放锁，唤醒后自动获得锁
        while (q.empty())
            pthread_cond_wait(&bqNotEmptyCond, &bqMutex);

        t = q.front();
        q.pop();

        // 解锁
        pthread_mutex_unlock(&bqMutex);

        // 唤醒由队列满而阻塞的线程
        pthread_cond_signal(&bqNotFullCond);
    }

    /** 返回队列当前 size */
    int queueSize()
    {
        return q.size();
    }

};


#endif //PIPELINE_BLOCKQUEUE_HPP
