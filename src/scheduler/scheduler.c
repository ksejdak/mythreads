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

int start_scheduler(schedulingpolicy_t scheduling_policy)
{
    // Set scheduling strategy.
    printf("Starting scheduler with policy: %s.\n", policy_to_str(scheduling_policy));
    scheduler.scheduling_policy = scheduling_policy;

    // Register handler for SIGALRM signal, that will preempt thread implementation.
    scheduler.signal_action.sa_handler = signal_handler;
    sigaction(SIGALRM, &scheduler.signal_action, NULL);

    // Start scheduler event loop.
    schedule_next_preemption();
}

void register_thread(mythread_t *thread)
{
    thread->state = STATE_NEW;

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
    sigaction(SIGALRM, &scheduler.signal_action, NULL);
    //ualarm(SCHEDULER_PREEMPTION_INTERVAL_USECS, 0);
    alarm(1);
}

void signal_handler(int sig)
{
    if(sig != SIGALRM)
        return;

    schedule_next_preemption();
    select_next_thread();
}

void select_next_thread()
{
    printf("Scheduling next thread.\n");

    threadnode_t *next_running_thread;
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        next_running_thread = roundrobin_next_running_thread();
        roundrobin_make_running(next_running_thread);
        break;
    }

    ucontext_t next_thread_context = next_running_thread->thread->context;
    if(swapcontext(&scheduler.swapped_context, &next_thread_context) == -1)
    {
        printf("Swapping context failed! Aborting program.\n");
        abort();
    }
}

