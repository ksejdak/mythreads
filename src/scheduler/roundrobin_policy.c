//============================================================================
//
// Filename   : roundrobin_policy.c
// Author     : Kuba Sejdak
// Created on : 20 Jan 2015
//
//============================================================================

#include "roundrobin_policy.h"
#include "scheduler_impl.h"
#include "utils/queue.h"

#include <stdlib.h>

threadnode_t *pending_threads_head;
threadnode_t *pending_threads_tail;

threadnode_t *ready_threads_head;
threadnode_t *ready_threads_tail;

threadnode_t *running_thread;

void roundrobin_make_pending(threadnode_t *thread_node)
{
    if(thread_node->thread->state == STATE_PENDING || thread_node->thread->state != STATE_RUNNING)
        return;

    // Remove this thread from 'running' handle.
    running_thread = NULL;

    // Make it 'pending'.
    if(pending_threads_head)
        queue_insert(thread_node, pending_threads_tail);
    else
        pending_threads_head = thread_node;

    pending_threads_tail = thread_node;
    thread_node->thread->state = STATE_PENDING;
}

void roundrobin_make_ready(threadnode_t *thread_node)
{
    if(thread_node->thread->state == STATE_READY)
        return;

    // Remove this thread from 'pending' queue, if it is 'pending'.
    if(thread_node->thread->state == STATE_PENDING)
    {
        if(thread_node == pending_threads_head)
            pending_threads_head = thread_node->next;

        if(thread_node == pending_threads_tail)
            pending_threads_tail = thread_node->prev;

        queue_remove(thread_node);
    }
    // Remove this thread from 'running' handle, if it is 'running'.
    else if(thread_node->thread->state == STATE_RUNNING)
        running_thread = NULL;

    // Make it 'ready'.
    if(ready_threads_head)
        queue_insert(thread_node, ready_threads_tail);
    else
        ready_threads_head = thread_node;

    ready_threads_tail = thread_node;
    thread_node->thread->state = STATE_READY;
}

void roundrobin_make_running(threadnode_t *thread_node)
{
    if(thread_node->thread->state == STATE_RUNNING)
        return;

    // Remove this thread from 'ready' queue.
    if(thread_node == ready_threads_head)
        ready_threads_head = thread_node->next;

    if(thread_node == ready_threads_tail)
        ready_threads_tail = thread_node->prev;

    queue_remove(thread_node);

    // Make this thread 'running'.
    running_thread = thread_node;
    thread_node->thread->state = STATE_RUNNING;
}

threadnode_t *roundrobin_next_running_thread()
{
    threadnode_t *first_ready_thread = ready_threads_head;
    if(!first_ready_thread)
        return running_thread;

    return first_ready_thread;
}

threadnode_t *roundrobin_remove_thread(int id)
{
    threadnode_t *iter;

    // Check in pending list.
    if(pending_threads_head)
    {
        for(iter = pending_threads_head; iter != NULL; iter = iter->next)
        {
            if(iter->thread->id == id)
            {
                if(iter == pending_threads_head)
                    pending_threads_head = iter->next;

                if(iter == pending_threads_tail)
                    pending_threads_tail = iter->prev;

                queue_remove(iter);
                return iter;
            }
        }
    }

    // Check in ready list.
    if(ready_threads_head)
    {
        for(iter = ready_threads_head; iter != NULL; iter = iter->next)
        {
            if(iter->thread->id == id)
            {
                if(iter == ready_threads_head)
                    ready_threads_head = iter->next;

                if(iter == ready_threads_tail)
                    ready_threads_tail = iter->prev;

                queue_remove(iter);
                return iter;
            }
        }
    }

    // Check in running handle.
    if(running_thread->thread->id == id)
    {
        threadnode_t *tmp = running_thread;
        running_thread = NULL;
        return tmp;
    }

    return NULL;
}

threadnode_t *roundrobin_get_pending_thread(mymutex_t *mutex)
{
    if(pending_threads_head)
    {
        threadnode_t *iter;
        for(iter = pending_threads_head; iter != NULL; iter = iter->next)
        {
            if(iter->thread->pending_mutex == mutex)
                return iter;
        }
    }

    // No thread is waiting for given mutex.
    return NULL;
}

int roundrobin_get_highest_priority(mymutex_t *mutex)
{
    int max_priority = -1;

    if(pending_threads_head)
    {
        threadnode_t *iter;
        for(iter = pending_threads_head; iter != NULL; iter = iter->next)
        {
            if(iter->thread->priority > max_priority)
                max_priority = iter->thread->priority;
        }
    }

    return max_priority;
}
