# Pipeline

## pthread

+ [Pthreads 入门教程 — My Blog (hanbingyan.github.io)](https://hanbingyan.github.io/2016/03/07/pthread_on_linux/#对线程的阻塞joining-and-detaching-threads)

### 创建线程

+ `pthread_t`: 线程唯一标识符，在Linux下是一个无符号长整型数据

+ 创建线程函数：

  + ```cpp
    int pthread_create (pthread_t *thread,pthread_attr_t *attr,void *(*start_routine)(void *),void *arg)
    ```

  + 参数

    + `pthread_t *thread`：thread, 线程标识符号。
      + 这个参数不是由用户指定的，而是由 pthread_create 函数在创建时将新的线程的标识符放到这个变量中
    + `pthread_attr_t *attr`：attr, 线程的属性。
      + 可以用 NULL 表示默认属性.
      + 线程在被创建时要被赋予一定的属性，这个属性存放在数据类型 pthread_attr_t 中，它包含了线程的调度策略，堆栈的相关信息，join or detach 的状态等.
    + `void *(*start_routine)(void *)`：start_routine, 线程开始运行的函数
    + `void *arg`：arg,是start_routine所需要的参数，是一个无类型的指针

  + 返回值

    + 创建成功，返回0；若出错，则返回错误编号

### 结束线程

+ 线程的结束情况
  + 线程运行的函数`return`了，也就是线程的任务已经完成
  + 线程调用了 `pthread_exit` 函数
  + 其他线程调用 `pthread_cancel` 结束这个线程
  + 进程调用 `exec() or exit()`，结束了
  + `main()` 函数先结束了，而且 `main()` 自己没有调用` pthread_exit` 来等所有线程完成任务
+ 一个线程结束，并不意味着它的所有信息都已经消失，后面会看到僵尸线程的问题.



+ `pthread_exit`

  + ```cpp
    void pthread_exit (void *retval)
    ```

  + retval 是由用户指定的参数， pthread_exit 完成之后可以通过这个参数获得线程的退出状态.

+ `pthread_cancel`

  + ```cpp
    int pthread_cancel (pthread_t thread)
    ```

  + 一个线程可以通过调用 pthread_cancel 函数来请求取消同一进程中的线程，这个线程由thread 参数指定. 如果操作成功则返回0，失败则返回对应的错误编号.



### 阻塞线程(Joining and Detaching Threads)

+ `pthread_join`

  + ```cpp
    int pthread_join(pthread_t threadid, void **value_ptr)
    ```

  + pthread_join 函数会让调用它的线程等待 threadid 线程运行结束之后再运行. value_ptr 存放了其他线程的返回值. 一个可以被join的线程，仅仅可以被别的一个线程 join，如果同时有多个线程尝试 join 同一个线程时，最终结果是未知的. 另外，线程不能 join 自己.

  + 创建一个线程时，要赋予它一定的属性，这其中就包括joinable or detachable 的属性，只有被声明成joinable的线程，可以被其他线程join

+ 僵尸线程

  + 一种已经退出了的 joinable 的线程，但是等待其他线程调用 pthread_join 来 join 它，以收集它的退出信息(exit status)。
  + 如果没有其他线程调用 pthread_join 来 join 它的话，它占用的一些系统资源不会被释放，比如堆栈. 如果main()函数需要长时间运行，并且创建大量 joinable 的线程，就有可能出现堆栈不足的 error .
  + 对于那些不需要 join 的线程，最好利用 pthread_detach，这样它运行结束后，资源就会及时得到释放. 注意一个线程被使用 pthread_detach 之后，它就不能再被改成 joinable 的了.
  + 创建的每一个线程都应该使用 pthread_join 或者 pthread_detach 其中一个，以防止僵尸线程的出现.
  + **`pthread_join()`和`pthread_detach()`：joinable的线程会等到调用pthread_join()的线程收集该线程的返回信息之后，才会释放资源，但是detachable的线程执行结束之后就自动释放资源了**

+ eg. 将线程设置为joinable的

  + ```cpp
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)
    rc = pthread_create(&thread, &attr, ThreadExecutionFunction, NULL);
    ```



### 堆栈管理(Stack Management)

+ POSIX标准没有规定一个线程的堆栈大小. 安全可移植的程序不会依赖于具体实现默认的堆栈限制，而是显式地调用 pthread_attr_setstacksize 来分配足够的堆栈空间.

+ 相关函数

  + ```cpp
    pthread_attr_getstacksize (attr, stacksize)
    pthread_attr_setstacksize (attr, stacksize)
    pthread_attr_getstackaddr (attr, stackaddr)
    pthread_attr_setstackaddr (attr, stackaddr)
    ```

+ eg. 设置堆栈大小

  + ```cpp
    pthread_t thread;
    pthread_attr_t attr;
    phtread_attr_init(&attr);
    
    // 为线程分配一个N*N的double 空间
    const int N = 1000;
    size_t stacksize = sizeof(double) * N * N;
    pthread_attr_setstacksize(&attr, stacksize);
    
    rc = pthread_create(&thread, &attr, ThreadExecutionFunction, NULL);
    
    ```

### 其他函数

+ `pthread_t pthread_self()`：返回自身的线程ID
+ `int pthread_equal(pthread_t t1, pthread_t t2)`: 比较两个线程的ID，相同返回非零值，不同返回0



### 互斥锁(Mutex)

+ Mutex 常常被用来保护那些可以被多个线程访问的共享资源，比如可以防止多个线程同时更新同一个数据时出现混乱.最后需要指出的是，保护共享数据是程序员的责任. 程序员要负责所有可以访问该数据的线程都使用mutex这种机制，否则，不使用 mutex 的线程还是有可能对数据造成破坏.

+ 使用互斥锁的步骤：

  + 创建一个互斥锁，即声明一个`pthread_mutex_t`类型的数据,然后初始化，只有初始化之后才能使用
  + 多个线程尝试锁定这个互斥锁
  + 只有一个成功锁定互斥锁，成为互斥锁的拥有者，然后进行一些指令
  + 拥有者解锁互斥锁
  + 其他线程尝试锁定这个互斥锁，重复上面的过程
  + 最后互斥锁被显式地调用 `pthread_mutex_destroy` 来进行销毁

+ 初始化互斥锁

  + 第一种，使用已经定义的常量初始化

    + ```cpp
      pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER
      ```

  + 第二种，使用`pthread_mutex_init(mutex, attr)`初始化

+ 当多个线程同时去锁定同一个互斥锁时，失败的那些线程，如果是用 `pthread_mutex_lock` 函数，那么会被阻塞，直到这个互斥锁被解锁，它们再继续竞争；如果是用 `pthread_mutex_trylock` 函数，那么失败者只会返回一个错误.

+ 相关函数

  + ```cpp
    pthread_mutex_init (mutex,attr)
    pthread_mutex_destroy (pthread_mutex_t *mutex)
    pthread_mutexattr_init (attr)
    pthread_mutexattr_destroy (attr)
    phtread_mutex_lock(pthread_mutex_t *mutex)
    phtread_mutex_trylock(pthread_mutex_t *mutex)
    phtread_mutex_unlock(pthread_mutex_t *mutex)
    ```



### 条件变量(Condition Variable)

+ 条件变量允许线程在阻塞的时候等待另一个线程发送的信号，当收到信号后，阻塞的线程就被唤醒并试图锁定与之相关的互斥锁. 条件变量要和互斥锁结合使用.

+ 条件变量初始化

  + 第一种，利用内部定义的常量

    + ```cpp
      pthread_cond_t myconvar = PTHREAD_COND_INITIALIZER;
      ```

  + 第二种，利用函数 pthread_cond_init(cond,attr),其中 attr 由 pthread_condattr_init() 和 pthread_condattr_destroy() 创建和销毁.

+ 相关函数

  ```cpp
  pthread_cond_wait (condition,mutex)
  pthread_cond_signal (condition)
  pthread_cond_broadcast (condition)
  ```

+ `pthread_cond_wait()`

  + pthread_cond_wait() 会阻塞调用它的线程，直到收到某一信号. 这个函数需要在 mutex 已经被锁之后进行调用，并且当线程被阻塞时，会自动解锁 mutex. 信号收到后，线程被唤醒，这时 mutex 又会被这个线程锁定.

+ `pthread_cond_signal()`

  + pthread_cond_signal() 函数结束时，必须解锁 mutex，以供 pthread_cond_wait() 锁定mutex. 当不止一个线程在等待信号时，要用 pthread_cond_broadcast() 代替 pthread_cond_signal() 来告诉所有被该条件变量阻塞的线程结束阻塞状态.



## 阻塞队列类(BlockQueue)

+ 阻塞队列用于多个线程对于队列这个临界资源的互斥访问。

+ 阻塞队列封装了对临界资源的互斥访问，外界调用阻塞队列的`push`或者`pop`时不用关心是否发生多个线程访问同一个队列的情况。相当于管程。

+ ```cpp
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
  
  ```

## 图像处理算法类(ImageAlgorithm)

+ 这个类可以作为各种图像处理算法的父类，某个特殊图像处理算法类继承此类后，可以重写图像处理算法，然后可以通过fu'l以达到多态的目的

+ ```cpp
  template <typename T>
  class ImageAlgorithm
  {
  
  public:
  
      virtual void algorithm(T &image){}
  
  };
  ```

+ 注意，父类需要被子类重写的方法需要被声明为 `virtual`.



## 水印算法类(Watermark)

+ 这个类继承于 ImageAlgorithm, 用于多态调用Watermark重写的父类图像图像处理算法

+ ```cpp
  template <typename T>
  class Watermark : public ImageAlgorithm<T>
  {
  private:
      int watermark;              // 打水印的数字
      std::string watermarkStr;   // 打水印的字符串
  
  public:
  
      Watermark(int watermark)
      {
          this->watermark = watermark;
          this->watermarkStr = std::to_string(watermark);
      }
  
      void algorithm(T &image)
      {
          /**
           * img : 编辑的图片
           * text : 要写入的字符串
           * org : 操作的坐标，cv::Point(列， 行)
           * fontFace : 字体类型
           * fontScale : 字体大小
           * color : 字体颜色
           * thickness : 字体厚度
           * */
          cv::putText(image,
                      watermarkStr,
                      cv::Point(image.cols / 3, image.rows / 4 + watermark * 120),
                      cv::FONT_HERSHEY_SIMPLEX,
                      4,
                      cv::Scalar(0, 0, 255),
                      5);
      }
  };
  ```

## 线程类(PipelineThread)

+ 流水线上的线程负责从其左边的阻塞队列中读取数据，经过其对应的图像处理算法操作后，再将数据放入其右边的阻塞队列

+ 注意 pthread_create 不能使用普通的成员函数作为线程函数，必须使用普通函数或者是静态成员函数

+ ```cpp
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
  ```

  

## 并发处理主函数(PipelineMain)

+ 此函数主要功能，一次性将所有图片读入内存，然后多线程执行图像处理算法，将所有图片处理完成后再一次性写到文件系统

+ ```cpp
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
  
  
  ```



## 串行处理主函数(SerialMain)

+ 一次性将所有图片读入内存，然后对每张图片串行执行图像处理算法，将所有图片处理完成后再一次性写到文件系统

+ ```cpp
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
  
  
  ```



## 实验结果对比

+ 并发处理是串行处理执行效率的两倍。

+ 并发

  + ```sh
    Program Consume Time = 0.01 s
    FPS = 100000
    ```

+ 串行

  + ```cpp
    Program Consume Time = 0.019 s
    FPS = 52631.6
    ```

    
