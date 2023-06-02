/*
 * @FilePath: \VsCode\workspace\threadpool\thread_pool.h
 * @ע�Ϳ�ʼ: ------------------------------------------------------------------------------
 * @��  Ȩ: (C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
 * @������: ����
 * @Date: 2023-06-02 16:54:40
 * @��  ��: 1.0
 * @��  ��: 
 * @Ӧ  ��: 
 * @ע�ͽ���: ------------------------------------------------------------------------------
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