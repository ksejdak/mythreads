//============================================================================
//
// Filename   : roundrobin_policy.h
// Author     : Kuba Sejdak
// Created on : 22 gru 2014
//
//============================================================================

#ifndef ROUNDROBINPOLICY_H
#define ROUNDROBINPOLICY_H

/// @brief Forwaring declaration of threadnode_t type.
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

#endif
