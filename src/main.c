//============================================================================
//
// Filename   : main.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"
#include "thread.h"
#include "mutex.h"

#include <stdio.h>

#define TEST_LOOP_ITERATIONS        300000

void func_1(void *data)
{
    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("1111 --- %d\n", i);
        fflush(stdout);

        if(i == (TEST_LOOP_ITERATIONS / 2))
            mythread_kill(2);
    }

    printf("Thread 1 loop done.\n");
    fflush(stdout);

    mythread_exit();
}

void func_2(void *data)
{
    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("2222 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 2 loop done.\n");
    fflush(stdout);

    mythread_exit();
}

void func_3(void *data)
{
    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("3333 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 3 loop done.\n");
    fflush(stdout);

    mythread_exit();
}

int main(int argc, char *argv[])
{
    printf("===== MyThread test app =====\n");

    // Create some threads.
    mythread_start(func_1, NULL);
    mythread_start(func_2, NULL);
    mythread_start(func_3, NULL);

    // This is blocking call.
    start_scheduler(POLICY_ROUND_ROBIN);

    return 0;
}
