#include <argp.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "util.h"

static uint16_t read_integer(char *string, char **next) {
    uint16_t result = 0;
    int sign = 1;

    if (*string == '+')
        string++;
    else if (*string == '-') {
        string++;
        sign = -1;
    }
    for (; *string >= '0' && *string <= '9'; string++) {
        result = (result * 10) + (*string - '0');
    }
    *next = string;
    if (sign >= 0) return result;
    return -result;
}

static int parse_geometry(char *geometry, int16_t *x, int16_t *y,
                          uint16_t *width, uint16_t *height) {
    /* Geometry format: [=][<width>{xX}<height>][{+-}<xoffset>{+-}<yoffset>] */

    char *next = NULL;

    if (*geometry == '=') geometry++; /* First character is an optional = */

    /* Read width */
    if (*geometry != '+' && *geometry != '-' && *geometry != 'x') {
        uint16_t width_temp = read_integer(geometry, &next);
        if (geometry == next) return 1;
        geometry = next;
        *width = width_temp;
    }

    /* Read height */
    if (*geometry == 'x' || *geometry == 'X') {
        geometry++;
        uint16_t height_temp = read_integer(geometry, &next);
        if (geometry == next) return 1;
        geometry = next;
        *height = height_temp;
    }

    /* Read x */
    if (*geometry == '-' || *geometry == '+') {
        int16_t x_temp = read_integer(geometry, &next);
        if (geometry == next) return 1;
        geometry = next;
        *x = x_temp;
    }

    /* Read y */
    if (*geometry == '-' || *geometry == '+') {
        int16_t y_temp = read_integer(geometry, &next);
        if (geometry == next) return 1;
        geometry = next;
        *y = y_temp;
    }

    if (*geometry != '\0') return 1;
    return 0;
}

static const char doc[] = "allim -- frontend for allimd";

static const struct argp_option options[] = {
    {"follow mouse", 'f', 0, 0, "Show window on selected monitor"},
    {"monitor", 'm', "MONITOR", 0, "Index of monitor to display window"},
    {"geometry", 'g', "GEOMETRY", 0, "Geometry of window"},
    {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct config *config = state->input;

    switch (key) {
        case 'f':
            config->follow_mouse = 1;
            break;
        case 'm':
            config->monitor = atoi(arg);
            break;
        case 'g':
            if (parse_geometry(arg, &config->window_x, &config->window_y,
                               &config->window_width, &config->window_height)) {
                die("Invalid geometry specification: %s", arg);
            }
            config->geometry = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const char args_doc[] = "test";

static struct argp argp = {options, parse_opt, args_doc, doc};

config get_config(int argc, char **argv) {
    config config;

    /* Set default values */
    config.follow_mouse = 0;
    config.monitor = 0;
    config.geometry = NULL;
    config.window_x = 30;
    config.window_y = 30;
    config.window_width = 300;
    config.window_height = 200;

    argp_parse(&argp, argc, argv, 0, 0, &config);

    return config;
}
