#include "err.h"
#include "readers_writers.h"

void init(ReadWrite *rw) {
    ASSERT_ZERO(sem_init(&rw->lock, 0, 1));
    ASSERT_ZERO(sem_init(&rw->readers, 0, 0));
    ASSERT_ZERO(sem_init(&rw->writers, 0, 0));
    rw->rcount = rw->wcount = rw->rwait = rw->wwait = 0;
}

void destroy(ReadWrite *rw) {
    ASSERT_ZERO(sem_destroy(&rw->lock));
    ASSERT_ZERO(sem_destroy(&rw->readers));
    ASSERT_ZERO(sem_destroy(&rw->writers));
}

void reader_start(ReadWrite *rw) {
    ASSERT_ZERO(sem_wait(&rw->lock));
    if (rw->wcount + rw->wwait > 0) {
        rw->rwait++;
        ASSERT_ZERO(sem_post(&rw->lock));
        ASSERT_ZERO(sem_wait(&rw->readers));
        rw->rwait--;
    }
    rw->rcount++;
    if (rw->rwait > 0) {
        ASSERT_ZERO(sem_post(&rw->readers));
    } else {
        ASSERT_ZERO(sem_post(&rw->lock));
    }
}

void reader_end(ReadWrite *rw) {
    ASSERT_ZERO(sem_wait(&rw->lock));
    rw->rcount--;
    if (rw->rcount == 0 && rw->wwait > 0) {
        ASSERT_ZERO(sem_post(&rw->writers));
    } else {
        ASSERT_ZERO(sem_post(&rw->lock));
    }
}

void writer_start(ReadWrite *rw) {
    ASSERT_ZERO(sem_wait(&rw->lock));
    if (rw->wcount + rw->rcount > 0) {
        rw->wwait++;
        ASSERT_ZERO(sem_post(&rw->lock));
        ASSERT_ZERO(sem_wait(&rw->writers));
        rw->wwait--;
    }
    rw->wcount++;
    ASSERT_ZERO(sem_post(&rw->lock));
}

void writer_end(ReadWrite *rw) {
    ASSERT_ZERO(sem_wait(&rw->lock));
    rw->wcount--;
    if (rw->wwait > 0) {
        ASSERT_ZERO(sem_post(&rw->writers));
    } else if (rw->rwait > 0) {
        ASSERT_ZERO(sem_post(&rw->readers));
    } else {
        ASSERT_ZERO(sem_post(&rw->lock));
    }
}
