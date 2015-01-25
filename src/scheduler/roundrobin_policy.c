//============================================================================
//
// Filename   : roundrobin_policy.c
// Author     : Kuba Sejdak
// Created on : 22 gru 2014
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
    if(thread_node->thread->state == STATE_PENDING)
        return;

    queue_remove(thread_node);

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

    if(thread_node->thread->state != STATE_NEW)
        queue_remove(thread_node);

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

    // Change state of currently running thread to 'ready'.
    if(running_thread)
    {
        queue_insert(running_thread, ready_threads_tail);
        ready_threads_tail = running_thread;
        running_thread->thread->state = STATE_READY;
    }

    running_thread = thread_node;
    thread_node->thread->state = STATE_RUNNING;
}

threadnode_t *roundrobin_next_running_thread()
{
    threadnode_t *first_ready_thread = ready_threads_head;
    if(!first_ready_thread)
    {
        printf("asdada\n");
        return running_thread;
    }

    ready_threads_head = first_ready_thread->next;
    queue_remove(first_ready_thread);

    return first_ready_thread;
}

void roundrobin_remove_thread(int id)
{
    threadnode_t *iter;

    // Check in pending list.
    if(pending_threads_head)
    {
        for(iter = pending_threads_head; iter->next != NULL; iter = iter->next)
        {
            if(iter->thread->id == id)
            {
                if(iter == pending_threads_head)
                {
                    pending_threads_head = iter->next;
                }
                if(iter == pending_threads_tail)
                {
                    pending_threads_tail = iter->prev;
                }
                queue_remove(iter);
                return;
            }
        }
    }

    // Check in ready list.
    if(ready_threads_head)
    {
        for(iter = ready_threads_head; iter->next != NULL; iter = iter->next)
        {
            if(iter->thread->id == id)
            {
                if(iter == ready_threads_head)
                {
                    ready_threads_head = iter->next;
                }
                if(iter == ready_threads_tail)
                {
                    ready_threads_tail = iter->prev;
                }
                queue_remove(iter);
                return;
            }
        }
    }
}
