//============================================================================
//
// Filename   : queue.c
// Author     : Kuba Sejdak
// Created on : 22 gru 2014
//
//============================================================================

#include "queue.h"

#include <stdio.h>

void queue_insert(threadnode_t *thread_node, threadnode_t *prev_node)
{
    thread_node->next = prev_node->next;
    prev_node->next = thread_node;
    thread_node->prev = prev_node;

    // When inserting between two nodes.
    if(thread_node->next)
        thread_node->next->prev = thread_node;
}

void queue_remove(threadnode_t *thread_node)
{
    if(thread_node->next)
        thread_node->next->prev = thread_node->prev;

    if(thread_node->prev)
        thread_node->prev->next = thread_node->next;

    thread_node->next = NULL;
    thread_node->prev = NULL;
}
