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
    pthread_t thread, thread_out, thread_err;
    pid_t pid;
    int task_no;
    char *out;
    char *err;
    sem_t mutex_out, mutex_err;
    int status;
    bool joined;
} Task;

Task *task_new(char **args, int task_no);

void print_task(Task *task);

void task_free(Task *task);
