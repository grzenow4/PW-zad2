#pragma once

#include "err.h"
#include "task.h"

#include <semaphore.h>

sem_t mutex;

typedef struct {
    int fd;
    Task *task;
} ReadThreadArgs;

void handle_run(Task **tasks, char **args, int task_no);

void handle_out(Task **tasks, int task_no);

void handle_err(Task **tasks, int task_no);

void handle_kill(Task **tasks, int task_no);

void handle_sleep(int n);

void handle_quit(Task **tasks);

void *read_stdout(void *data);

void *read_stderr(void *data);

void *run_task(void *data);

void free_tasks(Task **tasks);
