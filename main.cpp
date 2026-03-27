#include "ThreadPool.h"
#include "TaskQueue.h"
void han(void* arg){
    int* num=(int*) arg;
    std::cout<<*num<<' '<<pthread_self()<<std::endl;
    delete num;
    num=nullptr;
    sleep(3);
}
int main(){
    ThreadPool pool(1,10);
    for(int i=0;i<10;i++){
        int *num=new int(i);
        Task task(han,num);
        pool.addTask(task);
    }
    sleep(100);
    return 0;
}