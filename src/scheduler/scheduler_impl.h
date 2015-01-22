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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// TYPES
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

extern scheduler_t scheduler;

/// @brief Registers new tread in schedulers lists and initializes some its meta data.
/// @param [in] thread      thread handle to be registered
void register_thread(mythread_t *thread);

/// @brief Inserts newly created thread to appropriate
void enqueue_new_thread();

/// Registers next preemption by setting POSIX alarm.
void schedule_next_preemption();

/// @brief Handler of POSIX signals.
/// @param [in] sig         signal number
void signal_handler(int sig);

/// @brief Schedules next thread to be executed.
void select_next_thread();

#endif
