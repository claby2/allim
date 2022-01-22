#include <stddef.h>
#include <stdio.h>

#define MAX_BUF 1024

typedef struct config {
    int follow_mouse;
    int monitor;
} config;

typedef struct payload {
    char *header;
    char *text;
    struct config config;
} payload;

const char PIPE_PATH[] = "/tmp/allim.sock";

__attribute__((noreturn)) void die(const char *fmt, ...);

ssize_t read_checked(int fd, void *buf, size_t nbytes);

size_t str_marshal(void *dest, const char *src);
char *str_unmarshal(int fd);
