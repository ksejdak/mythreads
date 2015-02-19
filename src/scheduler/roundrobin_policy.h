//============================================================================
//
// Filename   : roundrobin_policy.h
// Author     : Kuba Sejdak
// Created on : 20 Jan 2015
//
//============================================================================

#ifndef ROUNDROBINPOLICY_H
#define ROUNDROBINPOLICY_H

#include "mutex.h"

/// @brief Forward declaration of threadnode_t type.
struct node;

/// @brief Changes state of given thread (in node representation) to 'pending'.
/// @param [i] thread_node          thread to be changed
void roundrobin_make_pending(struct node *thread_node);

/// @brief Changes state of given thread (in node representation) to 'ready'.
/// @param [i] thread_node          thread to be changed
void roundrobin_make_ready(struct node *thread_node);

/// @brief Changes state of given thread (in node representation) to 'running'.
/// @param [i] thread_node          thread to be changed
void roundrobin_make_running(struct node *thread_node);

/// @brief Executes Round Robin schduling algorithm to select next thread to be 'running'.
/// @return node representation of next running thread.
struct node *roundrobin_next_running_thread();

/// @brief Removes given thread node from lists.
/// @param [in] id                  ID of thread to be removed
/// @return Removed thread handle
struct node *roundrobin_remove_thread(int id);

/// @brief Returns thread waiting for given mutex.
/// @param [in] mutex       mutex, for which will be waiting returned thread
/// @return thread waiting for given mutex
struct node *roundrobin_get_pending_thread(mymutex_t *mutex);

/// @brief Checks what it the highest priority of all threads waiting for this mutex.
/// @param [in] mutex       mutex that is being locked
int roundrobin_get_highest_priority(mymutex_t *mutex);

#endif
