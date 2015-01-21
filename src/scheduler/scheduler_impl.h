//============================================================================
//
// Filename   : scheduler_impl.h
// Author     : Kuba Sejdak
// Created on : 21 gru 2014
//
//============================================================================

#ifndef SCHEDULERIMPL_H
#define SCHEDULERIMPL_H

#include "scheduler.h"
#include "thread/thread_impl.h"

#include <signal.h>
#include <ucontext.h>

#define SCHEDULER_PREEMPTION_INTERVAL_USECS     1000000

/// @brief Represents scheduler state and resources.
typedef struct
{
    schedulingpolicy_t scheduling_policy;
    struct sigaction signal_action;
    unsigned int threads_num;
    unsigned int next_id;
    ucontext_t swapped_context;
    mythread_t *current_thread;
} scheduler_t;

/// @brief Represents node in POSIX doubly-linked list.
typedef struct node
{
    struct node *forward;
    struct node *backward;
    mythread_t *thread;
} threadnode_t;

/// @brief Registers new tread in schedulers lists and initializes some its meta data.
/// @param [in] thread      thread handle to be registered
void register_thread(mythread_t *thread);

/// @brief Sets POSIX alarm to send SIGALRM after default chunk of time.
void schedule_next_preemption();

/// @brief Handler of POSIX signals.
/// @param [in] sig         signal number
void signal_handler(int sig);

/// @brief Schedules next thread to be executed.
void select_next_thread();

/// @brief Implements Round Robin policy for scheduling threads.
void exec_round_robin_policy();

/// @brief Implements FIFO policy for scheduling threads.
void exec_fifo_policy();

/// @brief Implements aging policy for scheduling threads.
void exec_aging_policy();

#endif
