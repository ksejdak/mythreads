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
#define TEST_MUTEX                  0

#if TEST_MUTEX
mymutex_t mutex;
#endif

void func_1(void *data)
{
#if TEST_MUTEX
    mymutex_lock(&mutex);
#endif

    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("1111 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 1 loop done.\n");
    fflush(stdout);

#if TEST_MUTEX
    mymutex_unlock(&mutex);
#endif
    mythread_exit();
}

void func_2(void *data)
{
#if TEST_MUTEX
    mymutex_lock(&mutex);
#endif

    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("2222 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 2 loop done.\n");
    fflush(stdout);

#if TEST_MUTEX
    mymutex_unlock(&mutex);
#endif
    mythread_exit();
}

void func_3(void *data)
{
#if TEST_MUTEX
    mymutex_lock(&mutex);
#endif

    int i;
    for(i = 0; i < TEST_LOOP_ITERATIONS; ++i)
    {
        printf("3333 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 3 loop done.\n");
    fflush(stdout);

#if TEST_MUTEX
    mymutex_unlock(&mutex);
#endif
    mythread_exit();
}

int main(int argc, char *argv[])
{
    printf("===== MyThread test app =====\n");

#if TEST_MUTEX
    mymutex_init(&mutex);
#endif

    // Create some threads.
    mythread_start(func_1, NULL);
    mythread_start(func_2, NULL);
    mythread_start(func_3, NULL);

    // This is blocking call.
    scheduler_start(POLICY_ROUND_ROBIN);

    return 0;
}
