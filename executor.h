#pragma once

#include "err.h"
#include "task.h"
#include "utils.h"

#include <vector>

#define MAX_N_TASKS 4096
#define MAX_OUT_LEN 1022
#define MAX_TASK_LEN 511

class Executor {
public:
    Executor() = default;

    void handle_run(const std::vector<std::string>& args);

    void handle_out(uint16_t task);

    void handle_err(uint16_t task);

    void handle_kill(uint16_t task);

    void handle_sleep(int n);

    void handle_quit();

private:
    std::vector<Task> _tasks;

    void add_task(const std::vector<std::string>& args);
};
