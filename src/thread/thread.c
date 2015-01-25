//============================================================================
//
// Filename   : thread.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "thread.h"
#include "thread_impl.h"
#include "scheduler/scheduler_impl.h"

#include <stdio.h>
#include <stdlib.h>

int mythread_start(void (*thread)(void *), void *args)
{
   mythread_t *new_thread = (mythread_t *) malloc(sizeof(mythread_t));

   // Register this thread in scheduler and initialize it with some meta data.
   register_thread(new_thread);

   // Obtain context of current thread.
   if(getcontext(&new_thread->context) == -1)
   {
       printf("Error in obtaining thread contex.\n");
       fflush(stdout);
       free(new_thread);
       return -1;
   }

   new_thread->context.uc_stack.ss_sp = new_thread->stack;
   new_thread->context.uc_stack.ss_size = THREAD_STACK_DEPTH;
   makecontext(&new_thread->context, thread, 1, args);

   printf("Created thread: '%s'.\n", new_thread->name);
   fflush(stdout);

   return new_thread->id;
}

int mythread_exit()
{
    int current_thread_id = scheduler.current_thread_node->thread->id;
    unregister_thread(current_thread_id);

    return 0;
}

int mythread_kill(int id)
{
    unregister_thread(id);

    return 0;
}
