#include "task.h"

Task *task_new(char **args, pid_t pid, size_t task_no) {
    Task *task = malloc(sizeof(Task));
    if (!task) {
        fatal("malloc failed");
    }
    task->args = args;
    task->pid = pid;
    task->task_no = task_no;
    task->running = true;
    task->out = calloc(1022, sizeof(char));
    task->err = calloc(1022, sizeof(char));
    ASSERT_ZERO(pthread_mutex_init(&task->mutex_out, NULL));
    ASSERT_ZERO(pthread_mutex_init(&task->mutex_err, NULL));
    return task;
}

void task_free(Task *task) {
    free_split_string(task->args);
    free(task->out);
    free(task->err);
    ASSERT_ZERO(pthread_mutex_destroy(&task->mutex_out));
    ASSERT_ZERO(pthread_mutex_destroy(&task->mutex_err));
    free(task);
}
