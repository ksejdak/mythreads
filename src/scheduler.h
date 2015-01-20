//============================================================================
//
// Filename   : scheduler.h
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread_structures.h"

/// @brief Represents supported scheduling policis, that can be executed by scheduler.
typedef enum
{
    POLICY_ROUND_ROBIN,
    POLICY_FIFO,
    POLICY_AGING
} schedulingpolicy_t;

/// @brief Initializes and starts scheduler.
int start_scheduler(schedulingpolicy_t scheduling_policy);

/// @brief Sets POSIX alarm for
void schedule_next_preemption();

/// @brief Represents node in POSIX doubly-linked list.
struct element
{
    struct element *forward;
    struct element *backward;
    mythread_t *thread;
};

struct element *pending_threads_list;
struct element *ready_threads_list;
struct element *running_threads_list;

#endif
