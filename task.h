#pragma once

#include "err.h"
#include "utils.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct {
    char **args;
    pid_t pid;
    size_t task_no;
    bool running;
    char *out;
    char *err;
    pthread_mutex_t mutex_out;
    pthread_mutex_t mutex_err;
} Task;

Task *task_new(char **args, pid_t pid, size_t task_no);

void task_free(Task *task);
