//============================================================================
//
// Filename   : scheduler.h
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#ifndef SCHEDULER_H
#define SCHEDULER_H

/// @brief Represents supported scheduling policis, that can be executed by scheduler.
typedef enum
{
    POLICY_ROUND_ROBIN,
    POLICY_FIFO,
    POLICY_AGING
} schedulingpolicy_t;

/// @brief Initializes and starts scheduler.
int start_scheduler(schedulingpolicy_t scheduling_policy);

#endif
