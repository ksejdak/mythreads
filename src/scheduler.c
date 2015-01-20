//============================================================================
//
// Filename   : scheduler.c
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#include "scheduler.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

char *policy_to_str(schedulingpolicy_t scheduling_policy)
{
    switch(scheduling_policy)
    {
    case POLICY_ROUND_ROBIN:    return "Round Robin";
    case POLICY_FIFO:           return "Fifo";
    case POLICY_AGING:          return "Aging";
    }

    assert(false);
}

int start_scheduler(schedulingpolicy_t scheduling_policy)
{
    printf("Starting scheduler with policy: %s.\n", policy_to_str(scheduling_policy));
}
