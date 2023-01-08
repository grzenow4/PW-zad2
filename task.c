#include "task.h"

Task *task_new(char **args, size_t task_no) {
    Task *task = malloc(sizeof(Task));
    if (!task) fatal("malloc failed");
    task->args = args;
    task->pid = 0;
    task->task_no = task_no;
    task->out = calloc(MAX_OUT_LEN, sizeof(char));
    task->err = calloc(MAX_OUT_LEN, sizeof(char));
    ASSERT_ZERO(pthread_mutex_init(&task->mutex_out, NULL));
    ASSERT_ZERO(pthread_mutex_init(&task->mutex_err, NULL));
    return task;
}

void task_free(Task *task) {
    ASSERT_ZERO(pthread_join(task->thread, NULL));
    free_split_string(task->args);
    free(task->out);
    free(task->err);
    ASSERT_ZERO(pthread_mutex_destroy(&task->mutex_out));
    ASSERT_ZERO(pthread_mutex_destroy(&task->mutex_err));
    free(task);
}
