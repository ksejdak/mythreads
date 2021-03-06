//============================================================================
//
// Filename   : queue.h
// Author     : Kuba Sejdak
// Created on : 20 Jan 2015
//
//============================================================================

#ifndef QUEUE_H
#define QUEUE_H

#include "thread/thread_impl.h"

/// @brief Represents node in POSIX doubly-linked list.
typedef struct node
{
    struct node *next;
    struct node *prev;
    mythread_t *thread;
} threadnode_t;

/// @brief Inserts list node into list, after given node.
/// @param [in] thread_node             node to be inserted
/// @param [in] prev_node               node, after which new node should be inserted
void queue_insert(threadnode_t *thread_node, threadnode_t *prev_node);

/// @brief Removes given node from list.
/// /// @param [in] thread_node             node to be removed
void queue_remove(threadnode_t *thread_node);

/// @brief Debugging function, that dumps whole queue to STDOUT.
/// @param [in] queue_head              head of this queue
/// @param [in] queue_name              name of this queue
void queue_dump(threadnode_t *queue_head, const char *queue_name);

#endif
