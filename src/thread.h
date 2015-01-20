//============================================================================
//
// Filename   : thread.h
// Author     : Kuba Sejdak
// Created on : 20 gru 2014
//
//============================================================================

#ifndef THREAD_H
#define THREAD_H

/// @brief Creates and starts new thread, that will execute given function with given parameters.
/// @param [in] thread          main thread function
/// @param [in] args            main thread function parameters
/// @return non-zero thread ID if thread was created successfully. 0 otherwise
int mythread_start(void (*thread)(void *), void *args);

/// @brief Terminates current thread nicely.
/// @return 0 if thread was terminated successfully. 1 otherwise
int mythread_exit();

/// @brief Terminates thread identified by given ID.
/// @param [in] id              ID of thread to be terminated
/// @return 0 if thread was terminated successfully. 1 otherwise
int mythread_kill(int id);

#endif
