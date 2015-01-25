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

#include <unistd.h>
#include <signal.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

scheduler_t scheduler;

char *policy_to_str(schedulingpolicy_t scheduling_policy)
{
    switch(scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:    return "Round Robin";
    }

    assert(false);
}

void start_scheduler(schedulingpolicy_t scheduling_policy)
{
    // Set scheduling strategy.
    printf("Starting scheduler with policy: %s.\n", policy_to_str(scheduling_policy));
    scheduler.scheduling_policy = scheduling_policy;

    // Register handler for SIGALRM signal, that will preempt thread implementation.
    scheduler.signal_action.sa_handler = signal_handler;
    sigaction(SIGALRM, &scheduler.signal_action, NULL);

    // Start scheduler event loop.
    schedule_next_preemption();
    select_next_thread();
    while(1);
}

void register_thread(mythread_t *thread)
{
    ++scheduler.threads_num;
    ++scheduler.next_id;

    thread->state = STATE_NEW;
    thread->id = scheduler.next_id;
    thread->was_preempted = false;

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
}

void unregister_thread(int id)
{
    // This is critical change in schedulers lists, so prevent preemption for now.
    alarm(0);

    printf("Terminating thread with id: %d.\n", id);
    fflush(stdout);

    bool terminate_current_thread = (scheduler.current_thread_node->thread->id == id);
    if(terminate_current_thread)
    {
        printf("Thread request to terminate itself. Very nice of him.\n");
        fflush(stdout);
        scheduler.current_thread_node = NULL;
    }
    else
    {
        switch(scheduler.scheduling_policy)
        {
        case POLICY_ROUND_ROBIN:
            roundrobin_remove_thread(id);
            break;
        }
    }

    --scheduler.threads_num;

    // Emulate SIGALRM signal.
    signal_handler(SIGALRM);
}

void schedule_next_preemption()
{
    printf("Scheduling next preemption.\n");
    fflush(stdout);

    sigaction(SIGALRM, &scheduler.signal_action, NULL);
    ualarm(SCHEDULER_PREEMPTION_INTERVAL_USECS, 0);
}

void signal_handler(int sig)
{
    if(sig != SIGALRM)
        return;

    printf("Received SIGALRM signal.\n");
    fflush(stdout);
    schedule_next_preemption();
    select_next_thread();
}

void select_next_thread()
{
    printf("Selecting next thread.\n");
    fflush(stdout);

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
            return;
        }
    }

    threadnode_t *next_running_thread;
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        next_running_thread = roundrobin_next_running_thread();
        //roundrobin_make_ready(scheduler.current_thread_node);
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
    if(swapcontext(&swapped_context, next_thread_context) == -1)
    {
        printf("Swapping context failed! Aborting program.\n");
        fflush(stdout);
        abort();
    }
}
