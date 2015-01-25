//============================================================================
//
// Filename   : mutex.h
// Author     : Kuba Sejdak
// Created on : 25 gru 2014
//
//============================================================================

#ifndef MUTEX_H
#define MUTEX_H

#include <stdbool.h>

struct node;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// TYPES
///////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Represents mutex to synchronize threads.
typedef struct
{
    bool locked;
    struct node *owner;
    struct node *pending_queue;
    int owner_original_priority;
} mymutex_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Initializes given mutex.
/// @note This function should be called, before using given mutex.
/// @param [in] mutex           handle to mutex, that will be initialized
void mymutex_init(mymutex_t *mutex);

/// @brief Locks given mutex. If it is already locked by other thread, then it blocks until it is released.
///        Thread is in such situation immediately preempted and changes state to 'pending'. It will not be
///        given any processor time, until mutex is unlocked.
///        Implemented mutex is not recursive, so more than one call to this function on the same mutex by
///        the same thread will result in error.
///        This is atomic operation, because scheduler is not preempting any other thread during this call.
/// @note This is not a timed lock. It permits deadlocks, if used incorrectly.
/// @param [in] mutex           handle to mutex, that will be locked
/// @return 0 on success, 1 otherwise
int mymutex_lock(mymutex_t *mutex);

/// @brief Unlocks given mutex.
///        This is atomic operation, because scheduler is not preempting any other thread during this call.
/// @note Only owner can unlock mutex. Any attempt to unlock someone's mutex will result in error.
/// @param [in] mutex           handle to mutex, that will be unlocked
/// @return 0 on success, 1 otherwise
int mymutex_unlock(mymutex_t *mutex);

#endif
