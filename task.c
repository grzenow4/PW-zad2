#include "task.h"

Task *task_new(char **args, int task_no) {
    Task *task = malloc(sizeof(Task));
    if (!task) fatal("malloc failed");
    task->args = args;
    task->pid = 0;
    task->task_no = task_no;
    task->out = calloc(MAX_OUT_LEN, sizeof(char));
    task->err = calloc(MAX_OUT_LEN, sizeof(char));
    ASSERT_ZERO(sem_init(&task->mutex_out, 0, 1));
    ASSERT_ZERO(sem_init(&task->mutex_err, 0, 1));
    task->joined = false;
    return task;
}

void print_task(Task *task) {
    if (!WIFEXITED(task->status)) {
        printf("Task %d ended: signalled.\n", task->task_no);
    } else {
        printf("Task %d ended: status %d.\n", task->task_no, WEXITSTATUS(task->status));
    }
}

void task_free(Task *task) {
    if (!task->joined) {
        ASSERT_ZERO(pthread_join(task->thread, NULL));
        task->joined = true;
        print_task(task);
    }
    free_split_string(task->args);
    free(task->out);
    free(task->err);
    ASSERT_ZERO(sem_destroy(&task->mutex_out));
    ASSERT_ZERO(sem_destroy(&task->mutex_err));
    free(task);
}
