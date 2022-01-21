#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/randr.h>
#include <xcb/xcb.h>

#include "../util.h"

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} monitor_info;

static xcb_connection_t *connection;
static xcb_screen_t *screen;
static xcb_window_t win;

/* Whether the notification should follow the mouse pointer */
static int follow_mouse = 1;
/* The monitor which should display the notifications */
static int monitor = 0;

static int16_t x = 30;
static int16_t y = 30;
static uint16_t width = 300;
static uint16_t height = 200;

static xcb_atom_t get_atom(char *name) {
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(
        connection, xcb_intern_atom(connection, 0, strlen(name), name), NULL);
    xcb_atom_t atom = reply->atom;
    free(reply);

    return atom;
}

static monitor_info get_monitor_info(void) {
    xcb_randr_get_screen_resources_current_reply_t *reply =
        xcb_randr_get_screen_resources_current_reply(
            connection,
            xcb_randr_get_screen_resources_current(connection, screen->root),
            NULL);

    xcb_timestamp_t timestamp = reply->config_timestamp;
    int len = xcb_randr_get_screen_resources_current_outputs_length(reply);
    xcb_randr_output_t *outputs =
        xcb_randr_get_screen_resources_current_outputs(reply);

    /* Keep track of current_monitor as current_monitor != i */
    int current_monitor = 0;
    for (int i = 0; i < len; ++i) {
        xcb_randr_get_output_info_reply_t *output =
            xcb_randr_get_output_info_reply(
                connection,
                xcb_randr_get_output_info(connection, outputs[i], timestamp),
                NULL);
        if (output == NULL || output->crtc == XCB_NONE ||
            output->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
            continue;

        xcb_randr_get_crtc_info_reply_t *crtc = xcb_randr_get_crtc_info_reply(
            connection,
            xcb_randr_get_crtc_info(connection, output->crtc, timestamp), NULL);
        monitor_info info = {.x = crtc->x,
                             .y = crtc->y,
                             .width = crtc->width,
                             .height = crtc->height};

        free(crtc);
        free(output);

        if (follow_mouse) {
            xcb_query_pointer_reply_t *pointer = xcb_query_pointer_reply(
                connection, xcb_query_pointer(connection, screen->root), NULL);
            /* Check if the pointer is within the bounds of the current monitor
             */
            if (pointer->root_x >= info.x &&
                pointer->root_x <= (info.x + info.width) &&
                pointer->root_y > info.y &&
                pointer->root_y <= (info.y + info.height))
                return info;
        } else if (current_monitor == monitor) {
            return info;
        }

        ++current_monitor;
    }
    die("Could not find valid monitor");
}

static void create_window(void) {
    win = xcb_generate_id(connection);

    monitor_info info = get_monitor_info();
    xcb_create_window(connection,                    /* conn */
                      screen->root_depth,            /* depth */
                      win,                           /* wid */
                      screen->root,                  /* parent */
                      x + info.x, y + info.y,        /* x, y */
                      width, height,                 /* width, height */
                      10,                            /* border_width */
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, /* _class */
                      screen->root_visual,           /* visual */
                      0,                             /* value_mask */
                      NULL                           /* value_list */
    );

    xcb_atom_t window_type_notification =
        get_atom("_NET_WM_WINDOW_TYPE_NOTIFICATION");
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, win,
                        get_atom("_NET_WM_WINDOW_TYPE"), XCB_ATOM_ATOM, 32,
                        sizeof(xcb_atom_t), &window_type_notification);

    xcb_map_window(connection, win);
}

int main(void) {
    connection = xcb_connect(NULL, NULL);
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    create_window();
    xcb_flush(connection);

    pause();

    return EXIT_SUCCESS;
}
