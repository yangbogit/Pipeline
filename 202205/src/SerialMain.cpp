#include <opencv2/opencv.hpp>
#include "PipelineThread.hpp"

#define IMAGE_NUM                   1000
#define IMAGE_PROCESS_THREAD_NUM    3
#define BLOCKQUEUE_MAXSIZE          100
#define BLOCKQUEUE_NUM              IMAGE_PROCESS_THREAD_NUM + 1

/********************* 串行处理实验 *****************************/

int main()
{
    /** 新建阻塞队列指针对象数组 */
    BlockQueue<cv::Mat> *blockQueue[BLOCKQUEUE_NUM];
    for (int i = 0; i < BLOCKQUEUE_NUM; i ++)
    {
        if (i == 0 || i == BLOCKQUEUE_NUM - 1)
            // 第1个和最后一个阻塞队列用于存放所有读取的图片和所有需要写出的图片
            // 所以容量要声明为 IMAGE_NUM, 最好再添加一定裕量
            blockQueue[i] = new BlockQueue<cv::Mat>(IMAGE_NUM + 10);
        else
            blockQueue[i] = new BlockQueue<cv::Mat>(BLOCKQUEUE_MAXSIZE);
    }

    /** 新建图像处理算法指针对象数组 */
    ImageAlgorithm<cv::Mat> *imageAlgorithm[IMAGE_PROCESS_THREAD_NUM];
    for (int i = 0; i < IMAGE_PROCESS_THREAD_NUM; i ++)
    {
        imageAlgorithm[i] = new Watermark<cv::Mat>(i);
    }

    /** 一次性读入所有图片到第一个阻塞队列中 */
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

    clock_t start_time = clock();

    /** 处理所有图片 */
    for (int i = 1; i <= IMAGE_NUM; i ++)
    {
        while (blockQueue[0]->queueSize())
        {
            cv::Mat image;
            blockQueue[0]->pop(image);
            for (int j = 0; j < IMAGE_PROCESS_THREAD_NUM; j ++)
                imageAlgorithm[j]->algorithm(image);
            blockQueue[BLOCKQUEUE_NUM - 1]->push(image);
        }
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

    // 程序执行结果
    for (auto bq : blockQueue)
    {
        std::cout << "BlockQueue Size = " <<  bq->queueSize() << " " << std :: endl;
    }
    double run_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << "Program Consume Time = " << run_time << " s" << std::endl;
    std::cout << "FPS = " << IMAGE_NUM / run_time << std::endl;

    return 0;

}

