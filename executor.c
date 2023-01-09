#include "executor.h"

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
        ASSERT_ZERO(sem_wait(&task->mutex_out));
        strcpy(task->out, buf);
        ASSERT_ZERO(sem_post(&task->mutex_out));
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
        ASSERT_ZERO(sem_wait(&task->mutex_err));
        strcpy(task->err, buf);
        ASSERT_ZERO(sem_post(&task->mutex_err));
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

    ReadThreadArgs args_out = {.fd = fd_out[0], .task = task};
    ReadThreadArgs args_err = {.fd = fd_err[0], .task = task};
    pthread_create(&task->thread_out, NULL, &read_stdout, &args_out);
    pthread_create(&task->thread_err, NULL, &read_stderr, &args_err);
    ASSERT_ZERO(sem_post(&task_mutex));

    int status;
    ASSERT_SYS_OK(waitpid(child, &status, 0));
    ASSERT_ZERO(pthread_join(task->thread_out, NULL));
    ASSERT_ZERO(pthread_join(task->thread_err, NULL));

    writer_start(&rw);
    task->status = status;
    ended_tasks[ended_tasks_size] = task->task_no;
    ended_tasks_size++;
    writer_end(&rw);
    return NULL;
}

void handle_run(Task **tasks, char **args, int task_no) {
    Task *task = task_new(args, task_no);
    tasks[task_no] = task;
    pthread_create(&task->thread, NULL, &run_task, task);
    ASSERT_ZERO(sem_wait(&task_mutex));
    printf("Task %d started: pid %d.\n", task->task_no, task->pid);
}

void handle_out(Task **tasks, int task_no) {
    ASSERT_ZERO(sem_wait(&tasks[task_no]->mutex_out));
    printf("Task %d stdout: '%s'.\n", task_no, tasks[task_no]->out);
    ASSERT_ZERO(sem_post(&tasks[task_no]->mutex_out));
}

void handle_err(Task **tasks, int task_no) {
    ASSERT_ZERO(sem_wait(&tasks[task_no]->mutex_err));
    printf("Task %d stderr: '%s'.\n", task_no, tasks[task_no]->err);
    ASSERT_ZERO(sem_post(&tasks[task_no]->mutex_err));
}

void handle_kill(Task **tasks, int task_no) {
    kill(tasks[task_no]->pid, SIGINT);
}

void handle_sleep(int n) {
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

void print_status(Task **tasks) {
    for (int i = 0; i < ended_tasks_size; i++) {
        Task *task = tasks[ended_tasks[i]];
        ASSERT_ZERO(pthread_join(task->thread, NULL));
        task->joined = true;
        print_task(task);
    }
    ended_tasks_size = 0;
}

int main() {
    Task **tasks = calloc(MAX_N_TASKS, sizeof(Task *));
    int tasks_size = 0;
    ASSERT_ZERO(sem_init(&task_mutex, 0, 0));
    init(&rw);
    ended_tasks = calloc(MAX_N_TASKS, sizeof(int));

    char *line = calloc(MAX_TASK_LEN, sizeof(char));

    while (read_line(line, MAX_TASK_LEN, stdin)) {
        char **parsed_line = split_string(line);

        reader_start(&rw);
        print_status(tasks);

        if (strcmp(parsed_line[0], "run") == 0) {
            handle_run(tasks, parsed_line, tasks_size);
            tasks_size++;
        } else if (strcmp(parsed_line[0], "out") == 0) {
            handle_out(tasks, (int) strtol(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "err") == 0) {
            handle_err(tasks, (int) strtol(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "kill") == 0) {
            handle_kill(tasks, (int) strtol(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "sleep") == 0) {
            handle_sleep((int) strtol(parsed_line[1], NULL, 10));
            free_split_string(parsed_line);
        } else if (strcmp(parsed_line[0], "quit") == 0) {
            free_split_string(parsed_line);
            reader_end(&rw);
            break;
        } else {
            free_split_string(parsed_line);
        }

        reader_end(&rw);
    }

    handle_quit(tasks);
    ASSERT_ZERO(sem_destroy(&task_mutex));
    free_tasks(tasks);
    destroy(&rw);
    free(ended_tasks);
    free(line);
    return 0;
}
