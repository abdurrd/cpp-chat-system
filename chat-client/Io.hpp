#pragma once
#include <sys/types.h>

struct Io {
        ssize_t reader(int sock, char *buffer, size_t nbyte);
        ssize_t writer(int sock, char *buffer, size_t nbyte);
};

//read(int, void *, size_t nbyte)
//send(int, const void *, size_t, int)

