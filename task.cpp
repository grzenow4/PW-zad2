#include "task.h"

Task::Task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no) {
    _args = args;
    _pid = pid;
    _task_no = task_no;
}
