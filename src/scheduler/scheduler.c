//============================================================================
//
// Filename   : scheduler.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"
#include "scheduler_impl.h"

#include <unistd.h>
#include <signal.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

static scheduler_t scheduler;

threadnode_t *pending_threads_head;
threadnode_t *pending_threads_tail;
threadnode_t *ready_threads_head;
threadnode_t *ready_threads_tail;
threadnode_t *running_threads_head;
threadnode_t *running_threads_tail;

char *policy_to_str(schedulingpolicy_t scheduling_policy)
{
    switch(scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:    return "Round Robin";
    case POLICY_FIFO:           return "Fifo";
    case POLICY_AGING:          return "Aging";
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
    ++scheduler.threads_num;
    ++scheduler.next_id;

    sprintf(thread->name, "Thread_%d", scheduler.next_id);
    threadnode_t *thread_node = (threadnode_t *) malloc(sizeof(threadnode_t));
    thread_node->thread = thread;

    // Each newly created thread is immediately in READY state.
    if(ready_threads_head)
        insque(thread_node, ready_threads_tail);
    else
        ready_threads_head = thread_node;

    ready_threads_tail = thread_node;
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
    switch(scheduler.scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:
        exec_round_robin_policy();
        break;
    case POLICY_FIFO:
        exec_fifo_policy();
        break;
    case POLICY_AGING:
        exec_aging_policy();
        break;
    }
}

void exec_round_robin_policy()
{
    threadnode_t *first_ready_thread = ready_threads_head;
    remque(first_ready_thread);
    ready_threads_head = first_ready_thread->forward;


    ucontext_t next_thread_context = read;

    if(swapcontext(&scheduler.swapped_context, &next_thread_context) == -1)
    {
        printf("Swapping context failed! Aborting program.\n");
        abort();
    }
}

void exec_fifo_policy()
{
}

void exec_aging_policy()
{
}
