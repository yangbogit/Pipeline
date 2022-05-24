#include <opencv2/opencv.hpp>
#include "PipelineThread.hpp"

#define IMAGE_NUM                   1000    // 图片数量设置
#define IMAGE_PROCESS_THREAD_NUM    3       // 线程数量设置
#define BLOCKQUEUE_MAXSIZE          100     // 非存放读入数据和写出数据的阻塞队列容量设置
#define BLOCKQUEUE_NUM              IMAGE_PROCESS_THREAD_NUM + 1    // 阻塞队列的数量

/********************* 并发处理实验 *****************************/

int main()
{
    /** 阻塞队列对象指针数组 */
    BlockQueue<cv::Mat> *blockQueue[BLOCKQUEUE_NUM];
    for (int i = 0; i < BLOCKQUEUE_NUM; i ++)
    {
        if (i == 0 || i == BLOCKQUEUE_NUM - 1)
            // 第 1 个和最后 1 个阻塞队列用于存放所有读取的图片和所有需要写出的图片
            // 所以其容量要声明为 IMAGE_NUM, 最好再添加一定裕量，防止发生意外
            blockQueue[i] = new BlockQueue<cv::Mat>(IMAGE_NUM + 10);
        else
            // 其余线程容量设置为指定容量
            blockQueue[i] = new BlockQueue<cv::Mat>(BLOCKQUEUE_MAXSIZE);
    }

    /** 读入所有图片到第 1 个阻塞队列 */
    for (int i = 1; i <= IMAGE_NUM; i ++)
    {
        std::string readPath = "../data/camera_in/camera_" + std::to_string(i) + ".jpg";
        cv::Mat image = cv::imread(readPath, cv::IMREAD_COLOR);
        if (!image.data)
        {
            std::cout << "Read Image Error !" << std::endl;
            return -1;
        }
        blockQueue[0]->push(image);
    }

    /** 图像处理算法对象指针数组 */
    // 一个线程对应一个图像处理算法
    ImageAlgorithm<cv::Mat> *imageAlgorithm[IMAGE_PROCESS_THREAD_NUM];
    for (int i = 0; i < IMAGE_PROCESS_THREAD_NUM; i ++)
    {
        imageAlgorithm[i] = new Watermark<cv::Mat>(i);
    }

    clock_t start_time = clock();

    /** 流水线线程对象指针数组 */
    // 启动多线程
    PipelineThread<cv::Mat> *pipelineThread[IMAGE_PROCESS_THREAD_NUM];
    for (int i = 0; i < IMAGE_PROCESS_THREAD_NUM; i ++)
    {
        pipelineThread[i] = new PipelineThread<cv::Mat>(IMAGE_NUM,
                                                        blockQueue[i],
                                                        blockQueue[i + 1],
                                                        imageAlgorithm[i]);
    }

    /** join所有线程，等待所有线程返回 */
    for (int i = 0; i < IMAGE_PROCESS_THREAD_NUM; i ++)
    {
        pipelineThread[i]->pipelineThreadJoin();
    }

    clock_t end_time = clock();

    /** 将所有处理过的图片写入文件 */
    for (int i = 1; i <= IMAGE_NUM; i ++)
    {
        cv::Mat image;
        blockQueue[BLOCKQUEUE_NUM - 1]->pop(image);
        std::string writePath = "../data/camera_out/camera_" + std::to_string(i) + ".jpg";
        cv::imwrite(writePath, image);
    }

    // 程序运行结果
    for (auto bq : blockQueue)
    {
        std::cout << "BlockQueue Size = " <<  bq->queueSize() << " " << std :: endl;
    }
    double run_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << "Program Consume Time = " << run_time << " s" << std::endl;
    std::cout << "FPS = " << IMAGE_NUM / run_time << std::endl;

    return 0;

}

