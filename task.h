#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include <vector>

class Task {
public:
    Task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no);

    ~Task();

    const std::vector<std::string> &get_args() const;

    pid_t get_pid() const;

    uint16_t get_task_no() const;

    const std::string &get_stdout() const;

    const std::string &get_stderr() const;

    void set_stdout(const std::string &out);

    void set_stderr(const std::string &err);

    void mutex_out_lock();

    void mutex_out_unlock();

    void mutex_err_lock();

    void mutex_err_unlock();

private:
    std::vector<std::string> _args;

    pid_t _pid;

    uint16_t _task_no;

    bool _running;

    std::string _stdout;

    std::string _stderr;

    pthread_mutex_t _mutex_out;

    pthread_mutex_t _mutex_err;
};
