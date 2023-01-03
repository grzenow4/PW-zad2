#pragma once

#include <iostream>
#include <string>
#include <vector>

class Task {
public:
    Task(const std::vector<std::string>& args);

private:
    std::vector<std::string> _args;
};
