#include "executor.h"

#include <csignal>
#include <iostream>
#include <sys/wait.h>

void Executor::add_task(const std::vector<std::string> &args, pid_t pid, uint16_t task_no) {
    _tasks.emplace_back(args, pid, task_no);
}

void Executor::handle_run(char** args) {
    std::vector<std::string> cargs;

    std::cout << "Wykonuję run";
    for (int i = 0; args[i] != NULL; i++) {
        std::cout << " " << args[i];
        cargs.emplace_back(args[i]);
    }
    std::cout << '\n';
    add_task(cargs, getpid(), _tasks.size());
}

void Executor::handle_out(uint16_t task) {
    std::cout << "Wykonuję out " << task << '\n';
}

void Executor::handle_err(uint16_t task) {
    std::cout << "Wykonuję err " << task << '\n';
}

void Executor::handle_kill(uint16_t task) {
    std::cout << "Wykonuję kill " << task << '\n';
}

void Executor::handle_sleep(int n) {
    usleep(1000 * n);
}

void Executor::handle_quit() {
    std::cout << "Wykonuję quit\n";
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
