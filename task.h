#pragma once

#include "err.h"
#include "utils.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define MAX_N_TASKS 4096
#define MAX_OUT_LEN 1022
#define MAX_TASK_LEN 511

typedef struct {
    char **args;
    pthread_t thread;
    pthread_t thread_out;
    pthread_t thread_err;
    pid_t pid;
    size_t task_no;
    char *out;
    char *err;
    sem_t mutex_out;
    sem_t mutex_err;
} Task;

Task *task_new(char **args, size_t task_no);

void task_free(Task *task);
