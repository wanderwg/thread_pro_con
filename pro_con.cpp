//实现生产者与消费者模型，模型的实现主要在于线程安全的队列操作
#include <iostream>
#include <queue>
#include <pthread.h>
#define MAX_QUEUE 10
class BlockQueue
{
public:
    BlockQueue(int cap=MAX_QUEUE)
        :_capacity(cap)
    {
        pthread_mutex_init(&_mutex,NULL);
        pthread_cond_init(&_cond_product,NULL);
        pthread_cond_init(&_cond_consumer,NULL);
    }
    ~BlockQueue(){
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_product);
        pthread_cond_destroy(&_cond_consumer);
    }
    bool QueuePush(int data){
        pthread_mutex_lock(&_mutex);
        while(_queue.size()==_capacity){
            pthread_cond_wait(&_cond_product,&_mutex);
        }
        _queue.push(data);
        pthread_cond_signal(&_cond_consumer);
        pthread_mutex_unlock(&_mutex);
    }
    bool QueuePop(int &data){
        pthread_mutex_lock(&_mutex);
        while(_queue.empty()){
            pthread_cond_wait(&_cond_consumer,&_mutex);
        }
        data=_queue.front();
        _queue.pop();
        pthread_cond_signal(&_cond_product);
        pthread_mutex_unlock(&_mutex);
    }
private:
    std::queue<int> _queue;
    int _capacity;//用于限制队列中最大节点数量
    pthread_mutex_t _mutex;
    pthread_cond_t _cond_product;
    pthread_cond_t _cond_consumer;
};

void*thr_consumer(void* arg)
{
    BlockQueue* q=(BlockQueue*)arg;
    while(1){
        int data;
        q->QueuePop(data);
        std::cout<<pthread_self()<<"get data:"<<data<<std::endl;
    }
    return NULL;
}

void*thr_product(void* arg)
{
    BlockQueue* q=(BlockQueue*)arg;
    int i=0;
    while(1){
        q->QueuePush(i++);
        std::cout<<pthread_self()<<"put data:"<<i<<std::endl;
    }
    return NULL;
}

int main()
{
    pthread_t con_tid[4],pro_tid[4];
    BlockQueue q;
    for(int i=0;i<4;++i){
        int ret=pthread_create(&con_tid[i],NULL,thr_consumer,(void*)&q);
        if(ret!=0){
            std::cout<<"thread create error"<<std::endl;
            return -1;
        }
    }

    for(int i=0;i<4;++i){
        int ret=pthread_create(&pro_tid[i],NULL,thr_product,(void*)&q);
        if(ret!=0){
            std::cout<<"thread create error"<<std::endl;
            return -1;
        }
    }

    for(int i=0;i<4;++i){
        pthread_join(con_tid[i],NULL);
    }
    for(int i=0;i<4;++i){
        pthread_join(pro_tid[i],NULL);
    }
    return 0;
}
