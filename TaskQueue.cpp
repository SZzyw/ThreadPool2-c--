#include "TaskQueue.h"

Task::Task(void (*function)(void *arg),void *arg):function(function),arg(arg){

}

TaskQueue::TaskQueue(){
}

TaskQueue::~TaskQueue(){
}

void TaskQueue::taskAdd(Task task){
    this->taskQueue.push(task);
}

Task TaskQueue::taskGet(){
    Task task(nullptr,nullptr);
    if(this->taskQueue.size()>0){
       task=this->taskQueue.front();
       taskQueue.pop();
    }
    return task;
}

int TaskQueue::taskSize(){
    int size=this->taskQueue.size();
    return size;
}
