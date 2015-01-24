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
#include <string.h>
#include <ucontext.h>

int mythread_start(void (*thread)(void *), void *args)
{
   mythread_t *new_thread = (mythread_t *) malloc(sizeof(mythread_t));

   // Register this thread in scheduler and initialize it with some meta data.
   register_thread(new_thread);

   // Obtain context of current thread.
   if(getcontext(&new_thread->context) == -1)
   {
       printf("Error in obtaining thread contex.\n");
       free(new_thread);
       return -1;
   }

   new_thread->context.uc_stack.ss_sp = new_thread->stack;
   new_thread->context.uc_stack.ss_size = THREAD_STACK_DEPTH;
   makecontext(&new_thread->context, thread, 0);

   printf("Created thread: '%s'.\n", new_thread->name);
}

int mythread_exit()
{
}

int mythread_kill(int id)
{
}
