#if !defined(TACKQUEUE)
#define TACKQUEUE

#include <iostream>
#include <pthread.h>
#include <queue>

class Task{
public:
    Task(void (*function)(void *arg),void *arg);
public:
    void (*function)(void *arg);//任务的函数
    void *arg;//任务的参数
};

class TaskQueue{
public:
    TaskQueue();

    ~TaskQueue();

    TaskQueue(const TaskQueue&)=delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    void taskAdd(Task task);//添加任务

    Task taskGet();//取出任务

    int taskSize();//获取队列的大小
private:
    std::queue<Task> taskQueue;//任务队列
    pthread_mutex_t queueMutex;//队列的锁
};


#endif // TACKQUEUE