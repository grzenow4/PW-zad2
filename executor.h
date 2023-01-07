#pragma once

#include "task.h"

#include <string.h>

const int MAX_N_TASKS = 4096;
const int MAX_OUT_LEN = 1022;
const int MAX_TASK_LEN = 511;

typedef struct {
    int fd;
    Task *task;
} ReadThreadArgs;

typedef struct {
    Task **tasks;
    char **args;
    size_t task_no;
} RunThreadArgs;

void handle_run(Task **tasks, char **args, size_t task_no);

void handle_out(Task **tasks, size_t task_no);

void handle_err(Task **tasks, size_t task_no);

void handle_kill(Task **tasks, size_t task_no);

void handle_sleep(size_t n);

void handle_quit();

void *read_stdout(void *data);

void *read_stderr(void *data);

void free_tasks(Task **tasks);
