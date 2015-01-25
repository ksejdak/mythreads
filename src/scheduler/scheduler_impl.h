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

/// @brief Returns id of current thread.
/// @return id of current thread or -1 if there is no running thread.
int get_current_thread_id();

/// @brief Disables preemption in scheduler.
void disable_preemption();

/// @brief Enables preemption in scheduler.
void enable_preemption();

/// Registers next preemption by setting POSIX alarm.
void schedule_next_preemption();

/// @brief Handler of POSIX signals.
/// @param [in] sig         signal number
void signal_handler(int sig);

/// @brief Schedules next thread to be executed.
void select_next_thread();

/// @brief Returns thread waiting for given mutex.
/// @param [in] mutex       mutex, for which will be waiting returned thread
/// @return thread waiting for given mutex
threadnode_t *get_pending_thread(mymutex_t *mutex);

/// @brief Changes state of given thread (in node representation) to 'pending'.
/// @param [i] thread_node          thread to be changed
void make_thread_pending(threadnode_t *thread_node);

/// @brief Changes state of given thread (in node representation) to 'ready'.
/// @param [i] thread_node          thread to be changed
void make_thread_ready(threadnode_t *thread_node);

#endif
