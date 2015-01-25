//============================================================================
//
// Filename   : scheduler.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"
#include "scheduler_impl.h"
#include "roundrobin_policy.h"
#include "utils/queue.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

scheduler_t scheduler;

char *policy_to_str(schedulingpolicy_t scheduling_policy)
{
    switch(scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:    return "Round Robin";
    }

    abort();
}

void scheduler_start(schedulingpolicy_t scheduling_policy)
{
    // Set scheduling strategy.
    printf("Starting scheduler with policy: %s.\n", policy_to_str(scheduling_policy));
    fflush(stdout);
    scheduler.scheduling_policy = scheduling_policy;

    // Register handler for SIGALRM signal, that will preempt thread implementation.
    scheduler.signal_action.sa_handler = scheduler_signal_handler;
    sigaction(SIGALRM, &scheduler.signal_action, NULL);

    // Start scheduler event loop.
    scheduler.started = true;

    // Obtain context of scheduler to be able to return here after all threads terminate.
    if(getcontext(&scheduler.context) == -1)
    {
        printf("Error in obtaining scheduler contex.\n");
        fflush(stdout);
        abort();
    }

    scheduler.context.uc_stack.ss_sp = scheduler.stack;
    scheduler.context.uc_stack.ss_size = THREAD_STACK_DEPTH;

    if(scheduler.threads_num > 0)
        scheduler_select_next_thread();
}

void scheduler_register_thread(mythread_t *thread)
{
    // This is critical change in schedulers queues, so prevent preemption for now.
    scheduler_disable_preemption();

    ++scheduler.threads_num;
    ++scheduler.next_id;

    thread->state = STATE_NEW;
    thread->id = scheduler.next_id;
    thread->was_preempted = false;
    thread->priority = 0;
    thread->pending_mutex = NULL;

    sprintf(thread->name, "Thread_%d", scheduler.next_id);
    threadnode_t *thread_node = (threadnode_t *) malloc(sizeof(threadnode_t));
    thread_node->thread = thread;

    // Each newly created thread is immediately made 'ready'.
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        roundrobin_make_ready(thread_node);
        break;
    }

    // This implies, that creating every new thread since scheduler was started is treated as preemption.
    scheduler_enable_preemption();
}

void scheduler_unregister_thread(int id)
{
    // This is critical change in schedulers queues, so prevent preemption for now.
    scheduler_disable_preemption();

    printf("Terminating thread with id: %d.\n", id);
    fflush(stdout);

    // Remove this thread from queues.
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        roundrobin_remove_thread(id);
        break;
    }

    bool terminate_current_thread = (scheduler.current_thread_node->thread->id == id);
    if(terminate_current_thread)
    {
        printf("Thread requests to terminate itself. Very nice of him.\n");
        fflush(stdout);
        scheduler.current_thread_node = NULL;
    }

    --scheduler.threads_num;

    // This implies, that terminating every thread since scheduler was started is treated as preemption.
    scheduler_enable_preemption();
}

int scheduler_get_current_thread_id()
{
    if(scheduler.current_thread_node)
        return scheduler.current_thread_node->thread->id;

    return -1;
}

void scheduler_disable_preemption()
{
    alarm(0);
}

void scheduler_enable_preemption()
{
    if(scheduler.started)
        scheduler_select_next_thread();
}

void scheduler_schedule_next_preemption()
{
    printf("Scheduling next preemption.\n");
    fflush(stdout);

    sigaction(SIGALRM, &scheduler.signal_action, NULL);
    ualarm(SCHEDULER_PREEMPTION_INTERVAL_USECS, 0);
}

void scheduler_signal_handler(int sig)
{
    if(sig != SIGALRM)
        return;

    printf("Received SIGALRM signal.\n");
    fflush(stdout);
    scheduler_select_next_thread();
}

void scheduler_select_next_thread()
{
    printf("Selecting next thread.\n");
    fflush(stdout);

    if(scheduler.threads_num == 0)
    {
        printf("All threads terminated. Exiting from scheduler.\n");
        fflush(stdout);
        ucontext_t swapped_context;
        swapcontext(&swapped_context, &scheduler.context);
        return;
    }

    if(scheduler.current_thread_node)
    {
        // This will be start point of this thread after it gets processor time again.
        getcontext(&scheduler.current_thread_node->thread->context);

        // Thread has returned from previous preemption. Continue its execution.
        if(scheduler.current_thread_node->thread->was_preempted)
        {
            scheduler.current_thread_node->thread->was_preempted = false;
            printf("Retrieved execution of thread: '%s'.\n", scheduler.current_thread_node->thread->name);
            fflush(stdout);

            // This thread will return to lock function. Don't schedule next preemption, because it has to be atomic.
            if(!scheduler.current_thread_node->thread->pending_mutex)
                scheduler_schedule_next_preemption();

            return;
        }
    }

    threadnode_t *next_running_thread;
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        next_running_thread = roundrobin_next_running_thread();

        if(scheduler.current_thread_node && scheduler.current_thread_node->thread->state != STATE_PENDING)
            roundrobin_make_ready(scheduler.current_thread_node);

        roundrobin_make_running(next_running_thread);
        break;
    }

    printf("Next running thread will be: '%s'.\n", next_running_thread->thread->name);
    fflush(stdout);

    if(scheduler.current_thread_node)
        scheduler.current_thread_node->thread->was_preempted = true;

    // Update schedulers handle to current thread.
    scheduler.current_thread_node = next_running_thread;

    ucontext_t *next_thread_context = &next_running_thread->thread->context;
    ucontext_t swapped_context;
    scheduler_schedule_next_preemption();
    if(swapcontext(&swapped_context, next_thread_context) == -1)
    {
        printf("Swapping context failed! Aborting program.\n");
        fflush(stdout);
        abort();
    }
}

void scheduler_notify_pending_thread(mymutex_t *mutex)
{
    threadnode_t *pending_thread;

    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        pending_thread = roundrobin_get_pending_thread(mutex);
        if(pending_thread)
            roundrobin_make_ready(pending_thread);

        break;
    }
}

int scheduler_get_highest_priority(mymutex_t *mutex)
{
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        return roundrobin_get_highest_priority(mutex);
    }
}

void scheduler_make_thread_pending(threadnode_t *thread_node)
{
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        roundrobin_make_pending(thread_node);
        break;
    }
}
