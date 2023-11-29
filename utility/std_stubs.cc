/**
 * @file std_stubs.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

// #include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <cstddef>

// Under gcc using virtual destructors, even when no malloc is involved,
// requires a delete operator to satisy the linker.
void operator delete(void*, size_t) {}

extern "C" {

int _fstat(int fd, struct stat *buf)
{
    (void)fd;
    (void)buf;
    return -1;
}

int _open(const char *path, int oflag, ...)
{
    (void)path;
    (void)oflag;
    return -1;
}

int _close(int fd)
{
    return -1;
}

ssize_t _read(int fd, void *buf, size_t nbyte)
{
    (void)fd;
    (void)buf;
    (void)nbyte;
    return -1;
}

ssize_t _write(int fd, const void *buf, size_t nbyte)
{
    (void)fd;
    (void)buf;
    (void)nbyte;
    return -1;
}

off_t _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return -1;
}

pid_t _getpid(void)
{
    return 0;
}

int _kill(pid_t pid, int sig)
{
    (void)pid;
    (void)sig;
    return -1;
}

 int _isatty(int fd)
 {
    (void)fd;
    return 0;
 }

}  // extern "C"