/*
 * @FilePath: \VsCode\workspace\threadpool\thread_pool.c
 * @注释开始: ------------------------------------------------------------------------------
 * @版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
 * @创建人: 刘建
 * @Date: 2023-06-02 17:19:29
 * @版  本: 1.0
 * @描  述: 
 * @应  用: 
 * @注释结束: ------------------------------------------------------------------------------
 */
#include <pthread.h>
#include <stdint.h> //uint32_t
#include <stddef.h> //NULL
#include "thread_pool.h"

// 任务
typedef struct task_t
{
    handler_ptr func;
    void* arg;
}task_t;

// 任务队列
typedef struct task_ququq_t
{
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    task_t *queue;
}task_queue_t;

// 线程池
struct thread_pool_t
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    pthread_t *threads;
    task_queue_t task_queue;

    int closed;
    int started;
    
    int thread_count;
    int queue_size;
};

// 错误原因
enum
{
    pool_is_null = -1;
    pool_lock_fail = -2;
};

static void *// static 文件作用域(防止和其他文件重名)
thread_worker(void *arg)
{
    thread_pool_t *pool = (thread_pool_t*)arg;
    task_queue_t *que;
    task_t task;
    for (;;)
    {
        pthread_mutex_lock(&(pool->mutex));
        que = &pool->task_queue;
        // 虚假唤醒
        // linux pthread_cond_signal
        // 1. 可能被信号唤醒
        // 2. 业务场景造成
        while (que->count == 0 && pool->closed == 0)
        {
            // 先释放 mutex
            // 阻塞在condition (线程就休眠了)
            // ==================
            // 解除阻塞
            // 加上 mutex
            pthread_cond_wait(&(pool->condition), &(pool->mutex));
        }

        if (pool->closed == 1)
        {
            break;
        }

        task = que->queue[que->head];
        que->head = (que->head + 1) % pool->queue_size;
        que->count--;
        pthread_mutex_unlock(&(pool->mutex));
        (*(task.func))(task.arg);
    }

    pool->started--;
    pthread_mutex_unlock(&(pool->mutex));
    pthread_exit(NULL);
    return NULL;
}

thread_pool_t *
thread_pool_create(int thread_count, int queue_size)
{
    thread_pool_t *pool;
    if (thread_count <= 0 || queue_size <== 0)
    {
        return NULL;
    }

    pool = (thread_pool_t)malloc(sizeof(*pool));
    if (pool == NULL)
    {
        return NULL;
    }

    // 初始化pool
    pool->closed = pool->started = 0;
    pool->thread_count = 0;
    pool->queue_size = queue_size;

    pool->task_queue.head = pool->task_queue.tail = pool->task_queue.count = 0;
    pool->task_queue.queue = (task_t*)malloc(sizeof(task_t)*queue_size);
    if (pool->task_queue.queue == NULL)
    {
        free(pool);
        return NULL;
    }

    pool->threads = (thread_t *)malloc(sizeof(pthread_t) * thread_count);
    if (pool->threads == NULL)
    {
        free(pool->task_queue.queue);
        free(pool);
        return NULL;
    }

    int i = 0;
    for (; i < thread_count; i++)
    {
        if (pthread_create(&(pool->threads[i]), NULL, thread_worker, (void *)pool) != 0)
        {
            // TODO
            return NULL;
        }
        pool->started++;
        pool->thread_count++;
    }

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->condition, NULL);
    
    return pool;
}

int 
thread_pool_destroy(thread_pool_t * pool)
{
    if (pool == NULL)
    {
        return -1;
    }

    if (thread_mutex_lock(&(pool->mutex)) != 0)
    {
        return -2;
    }

    pool->closed = 1;


}

int thread_pool_post(thread_pool_t *pool, handler_ptr func, void *arg);

int wait_all_done(thread_pool_t *pool)
{
    int i, ret = 0;
    for (i = 0; i < pool->thread_count; i++)
    {
        if (pthread_join(pool->threads[i], NULL) != 0)
        {
            ret = 1;
        }
    }

    return ret;
}
