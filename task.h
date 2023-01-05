#pragma once

#include <cstring>
#include <iostream>
#include <vector>

class Task {
public:
    Task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no);

private:
    std::vector<std::string> _args;
    pid_t _pid;
    uint16_t _task_no;
};
