//============================================================================
//
// Filename   : main.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"
#include "thread.h"

#include <unistd.h>
#include <stdio.h>

void func_1(void *data)
{
    int i;
    for(i = 0; i < 10000000; ++i)
    {
        printf("1111 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 1 loop done.");
    fflush(stdout);
    while(1);
}

void func_2(void *data)
{
    int i;
    for(i = 0; i < 10000000; ++i)
    {
        printf("2222 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 2 loop done.");
    fflush(stdout);
    while(1);
}

void func_3(void *data)
{
    int i;
    for(i = 0; i < 10000000; ++i)
    {
        printf("3333 --- %d\n", i);
        fflush(stdout);
    }

    printf("Thread 3 loop done.");
    fflush(stdout);
    while(1);
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
