#include "ThreadPool.h"

ThreadPool::ThreadPool(int minvalue, int maxvalue) : livenum(minvalue), busynum(0), minvalue(minvalue), maxvalue(maxvalue), exitvalue(0), shutdown(true)
{
    queue = new TaskQueue();

    pthread_mutex_init(&poolMutex, nullptr);
    pthread_mutex_init(&busyMutex, nullptr);
    pthread_cond_init(&isnull, nullptr);

    workerId.resize(maxvalue, 0);

    pthread_create(&managerId, nullptr, manager, this);
    for (int i = 0; i < minvalue; i++)
        pthread_create(&workerId[i], nullptr, worker, this);
}

void *ThreadPool::worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;

    while (true)
    {
        pthread_mutex_lock(&pool->poolMutex);
        while (pool->queue->taskSize() == 0 && pool->shutdown)
        {
            if (pool->exitvalue > 0)
            {
                pool->exitvalue--;
                pool->livenum--;
                pthread_mutex_unlock(&pool->poolMutex);
                pool->threadExit();
            }
            pthread_cond_wait(&pool->isnull, &pool->poolMutex);
        }
        if (pool->shutdown == false)
        {
            pthread_mutex_unlock(&pool->poolMutex);
            pool->threadExit();
        }
        Task task = pool->queue->taskGet();
        pthread_mutex_unlock(&pool->poolMutex);

        pthread_mutex_lock(&pool->busyMutex);
        pool->busynum++;
        pthread_mutex_unlock(&pool->busyMutex);

        task.function(task.arg);

        pthread_mutex_lock(&pool->busyMutex);
        pool->busynum--;
        pthread_mutex_unlock(&pool->busyMutex);
    }
}

void ThreadPool::threadExit()
{
    for (int i = 0; i < this->maxvalue; i++)
    {
        if (this->workerId[i] == pthread_self())
        {
            pthread_mutex_lock(&poolMutex);
            exitId.push_back(workerId[i]);
            this->workerId[i] = 0;
            pthread_mutex_unlock(&poolMutex);
            break;
        }
    }
    pthread_exit(nullptr);
}

void *ThreadPool::manager(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->shutdown)
    {
        sleep(3);

        pthread_mutex_lock(&pool->poolMutex);
        int livenum = pool->livenum;
        int exitIdSize = pool->exitId.size();
        pthread_mutex_unlock(&pool->poolMutex);
        pthread_mutex_lock(&pool->busyMutex);
        int busynum = pool->busynum;
        pthread_mutex_unlock(&pool->busyMutex);
        int maxvalue = pool->maxvalue;
        int minvalue = pool->minvalue;

        if (pool->queue->taskSize() > livenum && livenum < maxvalue)
        {
            pthread_mutex_lock(&pool->poolMutex);
            for (int i = 0; i < maxvalue && pool->shutdown; i++)
            {
                if (pool->workerId[i] == 0)
                {
                    pthread_create(&pool->workerId[i], nullptr, worker, pool);
                    pool->livenum++;
                    break;
                }
            }
            pthread_mutex_unlock(&pool->poolMutex);
        }

        if (busynum * 2 <= livenum && livenum > minvalue)
        {
            pthread_mutex_lock(&pool->poolMutex);
            pool->exitvalue = 1;
            pthread_mutex_unlock(&pool->poolMutex);
            pthread_cond_broadcast(&pool->isnull);
        }

        if (exitIdSize > 10)
        {
            std::vector<pthread_t> toJoin;
            pthread_mutex_lock(&pool->poolMutex);
            toJoin.swap(pool->exitId);
            pthread_mutex_unlock(&pool->poolMutex);

            for (int i = 0; i < toJoin.size(); i++)
            {
                pthread_join(toJoin[i], nullptr);
            }
        }
    }
    pthread_exit(nullptr);
}

void ThreadPool::addTask(Task task)
{
    if (this->shutdown == false)
    {
        return;
    }
    this->queue->taskAdd(task);
    pthread_cond_signal(&this->isnull);
}

int ThreadPool::getLivenum()
{
    pthread_mutex_lock(&this->poolMutex);
    int livenum = this->livenum;
    pthread_mutex_unlock(&this->poolMutex);
    return livenum;
}

int ThreadPool::getBugynum()
{
    pthread_mutex_lock(&this->busyMutex);
    int busynum = this->busynum;
    pthread_mutex_unlock(&this->busyMutex);
    return busynum;
}

ThreadPool::~ThreadPool()
{
    this->shutdown = false;
    pthread_cond_broadcast(&isnull);
    pthread_join(managerId, nullptr);
    for (int i = 0; i < this->maxvalue; i++)
    {
        if (this->workerId[i] != 0)
            pthread_join(this->workerId[i], nullptr);
    }
    for (int i = 0; i < this->exitId.size(); i++)
    {
        pthread_join(this->exitId[i], nullptr);
    }
    delete (this->queue);
    this->queue = nullptr;

    pthread_mutex_destroy(&poolMutex);
    pthread_mutex_destroy(&busyMutex);
    pthread_cond_destroy(&isnull);
}