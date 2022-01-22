#include <stdint.h>

typedef struct config {
    int follow_mouse;
    int monitor;
    char *geometry;
    int16_t window_x;
    int16_t window_y;
    uint16_t window_width;
    uint16_t window_height;
} config;

config get_config(int argc, char **argv);
