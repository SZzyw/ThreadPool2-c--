#if !defined(THREADPOOL)
#define THREADPOOL

#include "TaskQueue.h"
#include <pthread.h>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <vector>

class ThreadPool{

public:
    ThreadPool(int minvalue,int maxvalue);

    ~ThreadPool();

    void addTask(Task);//添加任务

    int getLivenum();//获取存活的线程数量

    int getBugynum();//获取正在忙的线程数量

private:
    static void* worker(void *arg);//工作线程函数

    static void* manager(void *arg);//管理线程函数

    void threadExit();//退出线程函数
private:
    TaskQueue *queue;//任务队列

    int livenum;//存活的线程数量
    int busynum;//正在忙的线程数量
    int minvalue;//最小线程数量
    int maxvalue;//最大线程数量
    int exitvalue;//需要删除的线程数量

    pthread_t managerId;//管理者线程
    std::vector<pthread_t> workerId;//工作者线程

    bool shutdown;//true正常运行，false停止运行

    pthread_mutex_t poolMutex;//锁整个线程
    pthread_mutex_t busyMutex;//锁busynum
    pthread_cond_t isnull;//锁任务队列为空
};

#endif // THREADPOOL
