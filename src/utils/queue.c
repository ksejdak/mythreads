//============================================================================
//
// Filename   : queue.c
// Author     : Kuba Sejdak
// Created on : 20 Jan 2015
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

void queue_dump(threadnode_t *queue_head, const char *queue_name)
{
    printf("Dumping queue: %s.\n", queue_name);
    fflush(stdout);

    if(!queue_head)
    {
        printf("Queue empty.\n");
        fflush(stdout);
        return;
    }

    threadnode_t *iter;
    for(iter = queue_head; iter != NULL; iter = iter->next)
    {
        printf("%s ", iter->thread->name);
        fflush(stdout);
    }

    printf("\n");
    fflush(stdout);
}
