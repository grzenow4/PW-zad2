#pragma once

extern "C" {
#include "err.h"
#include "utils.h"
}

#include "task.h"

const int MAX_N_TASKS = 4096;
const int MAX_OUT_LEN = 1022;
const int MAX_TASK_LEN = 511;

class Executor {
public:
    Executor() = default;

    void handle_run(char** args);

    void handle_out(uint16_t task);

    void handle_err(uint16_t task);

    void handle_kill(uint16_t task);

    void handle_sleep(int n);

    void handle_quit();

    void read_stdout(int fd, Task *task);

    void read_stderr(int fd, Task *task);

private:
    std::vector<Task> _tasks;

    void add_task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no);
};
