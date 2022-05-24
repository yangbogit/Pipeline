#ifndef PIPELINE_PIPELINETHREAD_HPP
#define PIPELINE_PIPELINETHREAD_HPP

#include "BlockQueue.hpp"
#include "Watermark.hpp"

/**
 * 流水线线程类：流水线上的线程
 * */

template <typename T>
class PipelineThread
{

private:
    int imageNum;                       // 图片数量
    BlockQueue<T> *popBLockQueue;       // 当前线程取数据的队列
    BlockQueue<T> *pushBlockQueue;      // 当前线程放数据的队列
    ImageAlgorithm<T> *imageAlgorithm;  // 当前线程使用的图像处理算法

    pthread_t thread;                   // 当前线程标识符

public:

    /** 构造函数 */
    PipelineThread(int imNum, BlockQueue<T> *popBQ, BlockQueue<T> *pushBQ, ImageAlgorithm<T> *imAlgorithm)
    {
        imageNum = imNum;
        popBLockQueue = popBQ;
        pushBlockQueue = pushBQ;
        imageAlgorithm = imAlgorithm;

        // pthread_create 不能使用普通的成员函数作为线程函数，必须使用普通函数或者是静态成员函数
        pthread_create(&thread, NULL, pipelineThread, (void *)this);
    }

    /** 线程执行函数 */
    static void *pipelineThread(void *arg)
    {
        T t;
        PipelineThread<T> *pipelineThread = (PipelineThread<T> *)arg;
        int imageNumber = pipelineThread->getImageNum();

        for (int i = 1; i <= imageNumber; i ++)
        {
            // 从上一个阻塞队列取数据
            pipelineThread->popBLockQueue->pop(t);

            // 处理当前图像
            pipelineThread->imageAlgorithm->algorithm(t);

            // 把处理好的图像放入下一个阻塞队列
            pipelineThread->pushBlockQueue->push(t);
        }
    }

    /** pthread_join */
    void pipelineThreadJoin()
    {
        pthread_join(thread, NULL);
    }

    /** 返回图像数量 */
    int getImageNum()
    {
        return imageNum;
    }
};

#endif //PIPELINE_PIPELINETHREAD_HPP
