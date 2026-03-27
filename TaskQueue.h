#if !defined(TACKQUEUE)
#define TACKQUEUE

#include <iostream>
#include <queue>

class Task{
public:
    Task(void (*function)(void *arg),void *arg);
public:
    void (*function)(void *arg);
    void *arg;
};

class TaskQueue{
public:
    TaskQueue();

    ~TaskQueue();

    TaskQueue(const TaskQueue&)=delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    void taskAdd(Task task);

    Task taskGet();

    int taskSize();
private:
    std::queue<Task> taskQueue;
};


#endif // TACKQUEUE
