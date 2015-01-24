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
    for(i = 0; i < 1000000; ++i)
        printf("1111111111111111 --- %d\n", i);

    while(1);
}

void func_2(void *data)
{
    int i;
    for(i = 0; i < 1000000; ++i)
        printf("2222222222222222 --- %d\n", i);

    while(1);
}

void func_3(void *data)
{
    int i;
    for(i = 0; i < 1000000; ++i)
        printf("3333333333333333 --- %d\n", i);

    while(1);
}

int main(int argc, char *argv[])
{
    printf("===== MyThread test app =====\n");

    //init();

    // Create some threads.
    mythread_start(func_1, NULL);
    mythread_start(func_2, NULL);
    mythread_start(func_3, NULL);

    // Start scheduler with given policy.
    start_scheduler(POLICY_ROUND_ROBIN);

    return 0;
}
