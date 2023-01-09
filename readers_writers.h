#pragma once

// Readers-writers problem with small change which allows readers starvation.

#include <semaphore.h>

typedef struct {
    sem_t lock, readers, writers;
    int rcount, wcount, rwait, wwait;
} ReadWrite;

void init(ReadWrite *rw);

void destroy(ReadWrite *rw);

void reader_start(ReadWrite *rw);

void reader_end(ReadWrite *rw);

void writer_start(ReadWrite *rw);

void writer_end(ReadWrite *rw);
