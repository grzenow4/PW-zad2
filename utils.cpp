#include "utils.h"
#include "err.h"

#include <cassert>
#include <cstring>
#include <fcntl.h>

void set_close_on_exec(int file_descriptor, bool value) {
    int flags = fcntl(file_descriptor, F_GETFD);
    ASSERT_SYS_OK(flags);
    if (value)
        flags |= FD_CLOEXEC;
    else
        flags &= ~FD_CLOEXEC;
    ASSERT_SYS_OK(fcntl(file_descriptor, F_SETFD, flags));
}

std::vector<std::string> split_string(const std::string &s) {
    size_t len = s.length();
    int spaces = 0;
    for (int i = 0; i < len; i++)
        if (s[i] == ' ')
            spaces++;
    std::vector<std::string> parts(spaces + 1);
    int p = 0;
    int b = 0;
    for (int i = 0; i < len; i++) {
        if (s[i] == ' ') {
            parts[p++] = s.substr(b, i - b);
            b = i + 1;
        }
    }
    parts[p++] = s.substr(b, len - b);
    assert(p == spaces + 1);
    return parts;
}

bool read_line(char *buffer, size_t size_of_buffer, FILE *file) {
    if (size_of_buffer < 2)
        fatal("Buffer too small: %d\n", size_of_buffer);

    char *line = nullptr;
    size_t n_bytes;
    ssize_t n_chars = getline(&line, &n_bytes, file);

    if (n_chars == -1) {
        if (ferror(file))
            syserr("Getline failed.");
        assert(feof(file));
        buffer[0] = '\0';
        return false;
    }

    if (n_chars == 0) {
        free(line);
        assert(feof(file));
        buffer[0] = '\0';
        return false;
    }

    size_t len = strlen(line);
    if (len < n_chars)
        fatal("Null character in input.");
    assert(n_chars == len);

    if (len + 1 > size_of_buffer)
        fatal("Line too long: %d > %d.", len, size_of_buffer - 1);
    memcpy(buffer, line, len + 1);

    free(line);

    return true;
}
