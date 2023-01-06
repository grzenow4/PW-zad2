#include "executor.h"

#include <sys/wait.h>
#include <thread>
#include <unistd.h>

void Executor::add_task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no) {
    _tasks.emplace_back(args, pid, task_no);
}

void Executor::read_stdout(int fd, Task *task) {
    char *buf = new char[MAX_OUT_LEN];
    FILE *file = fdopen(fd, "r");

    while (read_line(buf, MAX_OUT_LEN, file)) {
        task->mutex_out_lock();
        task->set_stdout(buf);
        task->mutex_out_unlock();
    }

    close(fd);
    delete[] buf;
}

void Executor::read_stderr(int fd, Task *task) {
    char *buf = new char[MAX_OUT_LEN];
    FILE *file = fdopen(fd, "r");

    while (read_line(buf, MAX_OUT_LEN, file)) {
        task->mutex_err_lock();
        task->set_stderr(buf);
        task->mutex_err_unlock();
    }

    close(fd);
    delete[] buf;
}

void Executor::handle_run(char** args) {
    std::vector<std::string> cargs;
    uint16_t task_no = _tasks.size();

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

        execvp(args[0], args);
        exit(0);
    }
    ASSERT_SYS_OK(close(fd_out[1]));
    ASSERT_SYS_OK(close(fd_err[1]));

    std::cout << "Task " << task_no << " started: pid " << child << ".\n";
    add_task(cargs, child, task_no);

    std::thread thread_out(&Executor::read_stdout, this, fd_out[0], &_tasks[task_no]);
    std::thread thread_err(&Executor::read_stderr, this, fd_err[0], &_tasks[task_no]);

    int res = waitpid(child, NULL, WNOHANG);
    thread_out.join();
    thread_err.join();

    std::cout << "Task " << task_no << " ended: status " << res << ".\n";
}

void Executor::handle_out(uint16_t task) {
    _tasks[task].mutex_out_lock();
    std::cout << "Task " << _tasks[task].get_task_no() << " stdout: '" << _tasks[task].get_stdout() << "'.\n";
    _tasks[task].mutex_out_unlock();
}

void Executor::handle_err(uint16_t task) {
    _tasks[task].mutex_err_lock();
    std::cout << "Task " << _tasks[task].get_task_no() << " stderr: '" << _tasks[task].get_stderr() << "'.\n";
    _tasks[task].mutex_err_unlock();
}

void Executor::handle_kill(uint16_t task) {
    // TODO
}

void Executor::handle_sleep(int n) {
    usleep(1000 * n);
}

void Executor::handle_quit() {
    // TODO
}

int main() {
    Executor executor = Executor();

    char *line = new char[MAX_TASK_LEN];

    while (read_line(line, MAX_TASK_LEN, stdin)) {
        char **parsed_line = split_string(line);

        if (strcmp(parsed_line[0], "run") == 0) {
            executor.handle_run(parsed_line + 1);
        } else if (strcmp(parsed_line[0], "out") == 0) {
            executor.handle_out(std::stoi(parsed_line[1]));
        } else if (strcmp(parsed_line[0], "err") == 0) {
            executor.handle_err(std::stoi(parsed_line[1]));
        } else if (strcmp(parsed_line[0], "kill") == 0) {
            executor.handle_kill(std::stoi(parsed_line[1]));
        } else if (strcmp(parsed_line[0], "sleep") == 0) {
            executor.handle_sleep(std::stoi(parsed_line[1]));
        } else if (strcmp(parsed_line[0], "quit") == 0) {
            executor.handle_quit();
        }

        free_split_string(parsed_line);
    }

    delete[] line;
    return 0;
}
