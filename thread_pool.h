/*
 * @FilePath: \VsCode\workspace\threadpool\thread_pool.h
 * @注释开始: ------------------------------------------------------------------------------
 * @版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
 * @创建人: 刘建
 * @Date: 2023-06-02 16:54:40
 * @版  本: 1.0
 * @描  述: 
 * @应  用: 
 * @注释结束: ------------------------------------------------------------------------------
 */
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

typedef struct thread_pool_t thread_pool_t;
typedef void (*handler_ptr)(void *);

thread_pool_t *thread_pool_create(int thread_count, int queue_size);

int thread_pool_destroy(thread_pool_t * pool);

int thread_pool_post(thread_pool_t *pool, handler_ptr func, void *arg);

int wait_all_done(thread_pool_t *pool);

#endif