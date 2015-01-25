//============================================================================
//
// Filename   : mutex.c
// Author     : Kuba Sejdak
// Created on : 25 gru 2014
//
//============================================================================

#include "mutex.h"
#include "scheduler/scheduler_impl.h"
#include "utils/queue.h"

#include <stdio.h>
#include <stdlib.h>

void mymutex_init(mymutex_t *mutex)
{
    mutex->locked = false;
    mutex->owner = NULL;
    mutex->pending_queue = NULL;
    mutex->owner_original_priority = 0;
}

int mymutex_lock(mymutex_t *mutex)
{
    if(!mutex)
        return 1;

    // This is critical change in schedulers queues, so prevent preemption for now.
    disable_preemption();

    printf("Locking mutex.\n");
    fflush(stdout);

    if(mutex->locked)
    {
        int owner_id = mutex->owner->thread->id;
        int current_thread_id = get_current_thread_id();
        if(owner_id == current_thread_id)
        {
            printf("Mutex is already locked this thread: '%s'.\n", scheduler.current_thread_node->thread->name);
            fflush(stdout);
            return 1;
        }

        // This mutex is locked by other thread. We need to wait.

        // Prevent priority inversion, by temporary increasing its priority to the highest priory
        // of all waiting for it threads.
        if(mutex->owner->thread->priority < scheduler.current_thread_node->thread->priority)
            mutex->owner->thread->priority = scheduler.current_thread_node->thread->priority;

        printf("Mutex is already locked by thread: '%s'. Blocking.\n", mutex->owner->thread->name);
        fflush(stdout);

        scheduler.current_thread_node->thread->pending_mutex = mutex;
        make_thread_pending(scheduler.current_thread_node);
        enable_preemption();

        // Here we know that scheduler woke us up due to unlocking mutex by its owner.
        // TODO: small race condition here !!!
        disable_preemption();
    }

    if(!mutex->locked)
    {
        mutex->locked = true;
        mutex->owner = scheduler.current_thread_node;
        mutex->owner_original_priority = scheduler.current_thread_node->thread->priority;
        printf("Mutex successfully locked by thread: '%s'.\n", scheduler.current_thread_node->thread->name);
        fflush(stdout);
    }

    // This implies, that terminating every thread since scheduler was started is treated as preemption.
    enable_preemption();
    return 0;
}

int mymutex_unlock(mymutex_t *mutex)
{
    if(!mutex)
        return 1;

    // This is critical change in schedulers queues, so prevent preemption for now.
    disable_preemption();

    int owner_id = mutex->owner->thread->id;
    int current_thread_id = get_current_thread_id();

    printf("Unlocking mutex.\n");
    fflush(stdout);
    if(owner_id != current_thread_id)
    {
        printf("Cannot unlock mutex by thread %d - it is owned by thread: %d.\n", current_thread_id, owner_id);
        fflush(stdout);
        // This implies, that terminating every thread since scheduler was started is treated as preemption.
        enable_preemption();
        return 1;
    }

    // Retrieve original priority of this thread.
    scheduler.current_thread_node->thread->priority = mutex->owner_original_priority;
    mutex->locked = false;

    threadnode_t *pending_thread =  get_pending_thread(mutex);
    make_thread_ready(pending_thread);

    // This implies, that terminating every thread since scheduler was started is treated as preemption.
    enable_preemption();
    return 0;
}
