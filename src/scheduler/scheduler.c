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
    thread->state = STATE_NEW;
    //thread->context.uc_link = &scheduler.context;

    ++scheduler.threads_num;
    ++scheduler.next_id;

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

void schedule_next_preemption()
{
    printf("Scheduling next preemption.\n");

    sigaction(SIGALRM, &scheduler.signal_action, NULL);
    ualarm(SCHEDULER_PREEMPTION_INTERVAL_USECS, 0);
}

void signal_handler(int sig)
{
    if(sig != SIGALRM)
        return;

    printf("Received SIGALRM signal.\n");
    schedule_next_preemption();
    select_next_thread();
}

void select_next_thread()
{
    printf("Selecting next thread.\n");

    threadnode_t *current_thread = roundrobin_currently_running_thread();
    if(current_thread)
        getcontext(&current_thread->thread->context);


    threadnode_t *next_running_thread;
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        next_running_thread = roundrobin_next_running_thread();
        roundrobin_make_running(next_running_thread);
        break;
    }

    printf("Next running thread will be: '%s'.\n", next_running_thread->thread->name);

    //if(current_thread)
    //    getcontext(&current_thread->thread->context);

    ucontext_t *next_thread_context = &next_running_thread->thread->context;
    ucontext_t swapped_context;
    if(swapcontext(&swapped_context, next_thread_context) == -1)
    {
        printf("Swapping context failed! Aborting program.\n");
        abort();
    }

    printf("DONE.\n");
}

