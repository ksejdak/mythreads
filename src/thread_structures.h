//============================================================================
//
// Filename   : thread_structures.h
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#ifndef THREADSTRUCTURES_H
#define THREADSTRUCTURES_H

#include <search.h>

#define THREAD_NAME_MAX_LENGTH      16

/// @brief Represents current state of thread in terms of scheduling.
typedef enum
{
    STATE_PENDING,
    STATE_READY,
    STATE_RUNNING
} taskstate_t;

typedef struct
{
    char name[THREAD_NAME_MAX_LENGTH];
    int priority;
    taskstate_t state;
} mythread_t;

#endif
