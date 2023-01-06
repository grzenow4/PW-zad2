#include "task.h"

Task::Task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no)
        : _args(args), _pid(pid), _task_no(task_no), _running(false),
          _stdout(""), _stderr("") {
    pthread_mutex_init(&_mutex_out, NULL);
    pthread_mutex_init(&_mutex_err, NULL);
}

Task::~Task() {
    pthread_mutex_destroy(&_mutex_out);
    pthread_mutex_destroy(&_mutex_err);
}

const std::vector<std::string> &Task::get_args() const {
    return _args;
}

pid_t Task::get_pid() const {
    return _pid;
}

uint16_t Task::get_task_no() const {
    return _task_no;
}

const std::string &Task::get_stdout() const {
    return _stdout;
}

const std::string &Task::get_stderr() const {
    return _stderr;
}

void Task::set_stdout(const std::string &out) {
    _stdout = out;
}

void Task::set_stderr(const std::string &err) {
    _stderr = err;
}

void Task::mutex_out_lock() {
    pthread_mutex_lock(&_mutex_out);
}

void Task::mutex_out_unlock() {
    pthread_mutex_unlock(&_mutex_out);
}

void Task::mutex_err_lock() {
    pthread_mutex_lock(&_mutex_err);
}

void Task::mutex_err_unlock() {
    pthread_mutex_unlock(&_mutex_err);
}
