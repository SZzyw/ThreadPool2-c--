#include "TaskQueue.h"

Task::Task(void (*function)(void *arg),void *arg):function(function),arg(arg){

}

TaskQueue::TaskQueue(){
    pthread_mutex_init(&queueMutex,nullptr);
}

TaskQueue::~TaskQueue(){
    pthread_mutex_destroy(&queueMutex);
}

void TaskQueue::taskAdd(Task task){
    pthread_mutex_lock(&queueMutex);
    this->taskQueue.push(task);
    pthread_mutex_unlock(&queueMutex);
}

Task TaskQueue::taskGet(){
    Task task(nullptr,nullptr);
    pthread_mutex_lock(&queueMutex); 
    if(this->taskQueue.size()>0){
       task=this->taskQueue.front();
       taskQueue.pop();
    }
    pthread_mutex_unlock(&queueMutex);
    return task;
}

int TaskQueue::taskSize(){
    pthread_mutex_lock(&queueMutex); 
    int size=this->taskQueue.size();
    pthread_mutex_unlock(&queueMutex);
    return size;
}