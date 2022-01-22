#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

__attribute__((noreturn)) void die(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputc('\n', stderr);

    exit(EXIT_FAILURE);
}

/* Error checks read call */
ssize_t read_checked(int fd, void *buf, size_t nbytes) {
    ssize_t n;
    if ((n = read(fd, buf, nbytes))) return n;
    perror("read");
    die("Failed to read pipe");
}

/* Marshal given string along with its size into destination */
/* Returns the size of memory that was copied */
size_t str_marshal(char *dest, const char *src) {
    size_t len = strlen(src);
    memcpy(dest, &len, sizeof(size_t));
    memcpy(dest + sizeof(size_t), src, len + 1);
    return sizeof(size_t) + len + 1;
}

/* Unmarshals string from given file descriptor */
char *str_unmarshal(int fd) {
    size_t size;
    char *str;
    read_checked(fd, &size, sizeof(size_t));
    str = malloc(size);
    read_checked(fd, str, size + 1);
    return str;
}
