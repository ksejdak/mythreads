//============================================================================
//
// Filename   : main.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("===== MyThread test app =====\n");

    start_scheduler(POLICY_ROUND_ROBIN);
    return 0;
}
