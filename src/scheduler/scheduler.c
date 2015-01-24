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
static bool initialized = 0;

char *policy_to_str(schedulingpolicy_t scheduling_policy)
{
    switch(scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:    return "Round Robin";
    }

    assert(false);
}

void register_thread(mythread_t *thread)
{
    thread->state = STATE_NEW;
    //hread->context.uc_sigmask = scheduler.signal_action.sa_mask;

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
    getcontext(&scheduler.current_thread->context);
    schedule_next_preemption();
    select_next_thread();
}

void select_next_thread()
{
    printf("Selecting next thread.\n");

    threadnode_t *next_running_thread;
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        next_running_thread = roundrobin_next_running_thread();
        roundrobin_make_running(next_running_thread);
        break;
    }

    printf("Next running thread will be: '%s'.\n", next_running_thread->thread->name);

    ucontext_t *next_thread_context = &next_running_thread->thread->context;

    if(!initialized)
    {
        printf("Saving scheduler context.\n");
        initialized = true;
        getcontext(&scheduler.scheduler_context);
    }
    else
    {
        printf("Saving thread context.\n");
        getcontext(&scheduler.current_thread->context);
    }
    printf("aa");

    scheduler.current_thread = next_running_thread->thread;
    ucontext_t *garbage;
    swapcontext(garbage, next_thread_context);

    printf("DONE: %d.\n", scheduler.current_thread->id);
}

