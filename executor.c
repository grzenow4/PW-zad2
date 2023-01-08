#include "executor.h"

#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void *read_stdout(void *data) {
    ReadThreadArgs *thread_args = data;
    int fd = thread_args->fd;
    Task *task = thread_args->task;

    char *buf = calloc(MAX_OUT_LEN, sizeof(char));
    FILE *file = fdopen(fd, "r");

    while (read_line(buf, MAX_OUT_LEN, file)) {
        ASSERT_ZERO(pthread_mutex_lock(&task->mutex_out));
        strcpy(task->out, buf);
        ASSERT_ZERO(pthread_mutex_unlock(&task->mutex_out));
    }

    ASSERT_ZERO(fclose(file));
    free(buf);
    return NULL;
}

void *read_stderr(void *data) {
    ReadThreadArgs *thread_args = data;
    int fd = thread_args->fd;
    Task *task = thread_args->task;

    char *buf = calloc(MAX_OUT_LEN, sizeof(char));
    FILE *file = fdopen(fd, "r");

    while (read_line(buf, MAX_OUT_LEN, file)) {
        ASSERT_ZERO(pthread_mutex_lock(&task->mutex_err));
        strcpy(task->err, buf);
        ASSERT_ZERO(pthread_mutex_unlock(&task->mutex_err));
    }

    ASSERT_ZERO(fclose(file));
    free(buf);
    return NULL;
}

void *run_task(void *data) {
    Task *task = data;

    int fd_out[2];
    int fd_err[2];
    ASSERT_SYS_OK(pipe(fd_out));
    ASSERT_SYS_OK(pipe(fd_err));

//    set_close_on_exec(fd_out[0], true);
//    set_close_on_exec(fd_out[1], true);
//    set_close_on_exec(fd_out[0], true);
//    set_close_on_exec(fd_err[1], true);

    pid_t child = fork();
    ASSERT_SYS_OK(child);
    if (!child) {
        dup2(fd_out[1], STDOUT_FILENO);
        dup2(fd_err[1], STDERR_FILENO);

        ASSERT_SYS_OK(close(fd_out[0]));
        ASSERT_SYS_OK(close(fd_out[1]));
        ASSERT_SYS_OK(close(fd_err[0]));
        ASSERT_SYS_OK(close(fd_err[1]));

        execvp(task->args[1], task->args + 1);
        exit(1);
    }
    ASSERT_SYS_OK(close(fd_out[1]));
    ASSERT_SYS_OK(close(fd_err[1]));

    task->pid = child;
    ASSERT_ZERO(pthread_mutex_unlock(&mutex));

    pthread_t thread_out, thread_err;
    ReadThreadArgs args_out = {.fd = fd_out[0], .task = task};
    ReadThreadArgs args_err = {.fd = fd_err[0], .task = task};
    pthread_create(&thread_out, NULL, &read_stdout, &args_out);
    pthread_create(&thread_err, NULL, &read_stderr, &args_err);

    int status;
    ASSERT_SYS_OK(waitpid(child, &status, 0));
    ASSERT_ZERO(pthread_join(thread_out, NULL));
    ASSERT_ZERO(pthread_join(thread_err, NULL));

    if (!WIFEXITED(status)) {
        printf("Task %zu ended: signalled.\n", task->task_no);
    } else {
        printf("Task %zu ended: status %d.\n", task->task_no, WEXITSTATUS(status));
    }
    return NULL;
}

void handle_run(Task **tasks, char **args, size_t task_no) {
    Task *task = task_new(args, task_no);
    tasks[task_no] = task;
    pthread_create(&task->thread, NULL, &run_task, task);
    ASSERT_ZERO(pthread_mutex_lock(&mutex));
    printf("Task %zu started: pid %d.\n", task->task_no, task->pid);
}

void handle_out(Task **tasks, size_t task_no) {
    ASSERT_ZERO(pthread_mutex_lock(&tasks[task_no]->mutex_out));
    printf("Task %zu stdout: '%s'.\n", task_no, tasks[task_no]->out);
    ASSERT_ZERO(pthread_mutex_unlock(&tasks[task_no]->mutex_out));
}

void handle_err(Task **tasks, size_t task_no) {
    ASSERT_ZERO(pthread_mutex_lock(&tasks[task_no]->mutex_err));
    printf("Task %zu stderr: '%s'.\n", task_no, tasks[task_no]->err);
    ASSERT_ZERO(pthread_mutex_unlock(&tasks[task_no]->mutex_err));
}

void handle_kill(Task **tasks, size_t task_no) {
    kill(tasks[task_no]->pid, SIGINT);
}

void handle_sleep(size_t n) {
    usleep(1000 * n);
}

void handle_quit(Task **tasks) {
    for (int i = 0; tasks[i] != NULL; i++) {
        kill(tasks[i]->pid, SIGKILL);
    }
}

void free_tasks(Task **tasks) {
    for (int i = 0; tasks[i] != NULL; i++) {
        task_free(tasks[i]);
    }
    free(tasks);
}

int main() {
    Task **tasks = calloc(MAX_N_TASKS, sizeof(Task *));
    size_t tasks_size = 0;
    ASSERT_ZERO(pthread_mutex_init(&mutex, NULL));
    ASSERT_ZERO(pthread_mutex_lock(&mutex));

    char *line = calloc(MAX_TASK_LEN, sizeof(char));

    while (read_line(line, MAX_TASK_LEN, stdin)) {
        char **parsed_line = split_string(line);

        if (strcmp(parsed_line[0], "run") == 0) {
            handle_run(tasks, parsed_line, tasks_size);
            tasks_size++;
        } else if (strcmp(parsed_line[0], "out") == 0) {
            handle_out(tasks, strtoul(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "err") == 0) {
            handle_err(tasks, strtoul(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "kill") == 0) {
            handle_kill(tasks, strtoul(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "sleep") == 0) {
            handle_sleep(strtoul(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "quit") == 0) {
            free_split_string(parsed_line);
            break;
        } else {
            free_split_string(parsed_line);
        }
    }

    handle_quit(tasks);
    ASSERT_ZERO(pthread_mutex_unlock(&mutex));
    ASSERT_ZERO(pthread_mutex_destroy(&mutex));
    free_tasks(tasks);
    free(line);
    return 0;
}
