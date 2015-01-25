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
    mutex->owner_original_priority = 0;
}

int mymutex_lock(mymutex_t *mutex)
{
    if(!mutex)
        return 1;

    // This is critical change in schedulers queues, so prevent preemption for now.
    scheduler_disable_preemption();

    const char *current_thread_name = scheduler.current_thread_node->thread->name;
    printf("Locking mutex by thread: '%s'.\n", current_thread_name);
    fflush(stdout);

    if(mutex->locked)
    {
        int owner_id = mutex->owner->thread->id;
        int current_thread_id = scheduler_get_current_thread_id();
        if(owner_id == current_thread_id)
        {
            printf("Mutex is already locked this thread: '%s'.\n", current_thread_name);
            fflush(stdout);
            // This implies, that locking any thread since scheduler was started is treated as preemption.
            scheduler_enable_preemption();
            return 2;
        }

        // This mutex is locked by other thread. We need to wait.

        // Prevent priority inversion, by temporary increasing its priority to the highest priory
        // of all waiting for it threads.
        if(mutex->owner->thread->priority < scheduler.current_thread_node->thread->priority)
            mutex->owner->thread->priority = scheduler.current_thread_node->thread->priority;

        printf("Mutex is already locked by thread: '%s'. Blocking.\n", mutex->owner->thread->name);
        fflush(stdout);

        scheduler.current_thread_node->thread->pending_mutex = mutex;
        scheduler_make_thread_pending(scheduler.current_thread_node);
        scheduler_enable_preemption();

        // Here we know that scheduler woke us up due to unlocking mutex by its owner.
        // Scheduler also knows, that we were waiting for mutex inside locking function and
        // it didn't enable scheduling, so no need to disable it here.
    }

    if(!mutex->locked)
    {
        mutex->locked = true;
        mutex->owner = scheduler.current_thread_node;
        mutex->owner_original_priority = scheduler.current_thread_node->thread->priority;

        // Get highest priority of all still waiting threads for this mutex (to prevent from priority inversion).
        int highest_waiting_priority = scheduler_get_highest_priority(mutex);
        if(highest_waiting_priority > scheduler.current_thread_node->thread->priority)
            scheduler.current_thread_node->thread->priority = highest_waiting_priority;

        printf("Mutex successfully locked by thread: '%s'.\n", current_thread_name);
        fflush(stdout);
    }

    // This implies, that locking any thread since scheduler was started is treated as preemption.
    scheduler_enable_preemption();
    return 0;
}

int mymutex_unlock(mymutex_t *mutex)
{
    if(!mutex)
        return 1;

    // This is critical change in schedulers queues, so prevent preemption for now.
    scheduler_disable_preemption();

    int owner_id = mutex->owner->thread->id;
    int current_thread_id = scheduler_get_current_thread_id();

    printf("Unlocking mutex by thread: '%s'.\n", scheduler.current_thread_node->thread->name);
    fflush(stdout);
    if(owner_id != current_thread_id)
    {
        printf("Cannot unlock mutex by thread %d - it is owned by thread: %d.\n", current_thread_id, owner_id);
        fflush(stdout);
        // This implies, that terminating every thread since scheduler was started is treated as preemption.
        scheduler_enable_preemption();
        return 2;
    }

    // Restore all values and release mutex.
    scheduler.current_thread_node->thread->priority = mutex->owner_original_priority;
    scheduler.current_thread_node->thread->pending_mutex = NULL;
    mutex->locked = false;
    mutex->owner = NULL;
    mutex->owner_original_priority = 0;

    // Notify any pending thread, that it has been unlocked.
    scheduler_notify_pending_thread(mutex);

    printf("Mutex successfully unlocked by thread: '%s'.\n", scheduler.current_thread_node->thread->name);
    fflush(stdout);

    // This implies, that unlocking any thread since scheduler was started is treated as preemption.
    scheduler_enable_preemption();
    return 0;
}
