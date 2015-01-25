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
#include "utils/queue.h"

#include <signal.h>
#include <ucontext.h>

#define SCHEDULER_PREEMPTION_INTERVAL_USECS     10000

///////////////////////////////////////////////////////////////////////////////////////////////////
/// TYPES
///////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Represents scheduler state and resources.
typedef struct
{
    bool started;
    schedulingpolicy_t scheduling_policy;
    struct sigaction signal_action;
    unsigned int threads_num;
    unsigned int next_id;
    threadnode_t *current_thread_node;
    ucontext_t context;
    char stack[THREAD_STACK_DEPTH];
} scheduler_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

extern scheduler_t scheduler;

/// @brief Registers new tread in schedulers lists and initializes some its meta data.
/// @param [in] thread      thread handle to be registered
void register_thread(mythread_t *thread);

/// @brief Unregisters tread from schedulers lists and terminates it.
/// @param [in] id          id of thread to be terminated
void unregister_thread(int id);

/// Registers next preemption by setting POSIX alarm.
void schedule_next_preemption();

/// @brief Handler of POSIX signals.
/// @param [in] sig         signal number
void signal_handler(int sig);

/// @brief Schedules next thread to be executed.
void select_next_thread();

#endif
