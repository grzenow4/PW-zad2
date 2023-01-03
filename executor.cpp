#include "executor.h"

#include <iostream>

void Executor::add_task(const std::vector<std::string> &args) {
    _tasks.emplace_back(args);
}

void Executor::handle_run(const std::vector<std::string> &args) {
    std::cout << "Wykonuję run";
    for (const auto &arg : args) {
        std::cout << " " << arg;
    }
    std::cout << '\n';
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
    std::cout << "Wykonuję sleep " << n << '\n';
}

void Executor::handle_quit() { std::cout << "Wykonuję quit\n"; }

int main() {
    Executor executor = Executor();

    std::string line;
    std::vector<std::string> parsed_line;

    while (getline(std::cin, line)) {
        parsed_line = split_string(line);

        if (parsed_line.empty()) {
            continue;
        }

        if (parsed_line[0] == "run") {
            parsed_line.erase(parsed_line.begin());
            executor.handle_run(parsed_line);
        } else if (parsed_line[0] == "out") {
            executor.handle_out(std::stoi(parsed_line[1]));
        } else if (parsed_line[0] == "err") {
            executor.handle_err(std::stoi(parsed_line[1]));
        } else if (parsed_line[0] == "kill") {
            executor.handle_kill(std::stoi(parsed_line[1]));
        } else if (parsed_line[0] == "sleep") {
            executor.handle_sleep(std::stoi(parsed_line[1]));
        } else if (parsed_line[0] == "quit") {
            executor.handle_quit();
        }
    }

    return 0;
}
