//============================================================================
//
// Filename   : thread_structures.h
// Author     : Kuba Sejdak
// Created on : 20 Jan 2015
//
//============================================================================

#ifndef THREADSTRUCTURES_H
#define THREADSTRUCTURES_H

#include "mutex.h"

#include <stdbool.h>
#include <ucontext.h>

#define THREAD_NAME_MAX_LENGTH      16
#define THREAD_STACK_DEPTH          16384

///////////////////////////////////////////////////////////////////////////////////////////////////
/// TYPES
///////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Represents current state of thread in terms of scheduling.
typedef enum
{
    STATE_NEW,
    STATE_PENDING,
    STATE_READY,
    STATE_RUNNING,
    STATE_DEAD
} taskstate_t;

// @brief Represents thread handle and all its resources.
typedef struct
{
    int id;
    char name[THREAD_NAME_MAX_LENGTH];
    char stack[THREAD_STACK_DEPTH];
    int priority;
    bool was_preempted;
    taskstate_t state;
    ucontext_t context;
    mymutex_t *pending_mutex;
} mythread_t;

#endif
