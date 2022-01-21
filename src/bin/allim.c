#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xcb.h>

xcb_connection_t *c;
xcb_screen_t *screen;
xcb_window_t win;

int16_t x = 30;
int16_t y = 30;
int16_t width = 300;
int16_t height = 200;

xcb_atom_t get_atom(char *name) {
  xcb_atom_t atom;
  xcb_intern_atom_cookie_t atom_cookie;
  xcb_intern_atom_reply_t *reply;

  atom_cookie = xcb_intern_atom(c, 0, strlen(name), name);
  reply = xcb_intern_atom_reply(c, atom_cookie, NULL);

  atom = reply->atom;
  free(reply);

  return atom;
}

void create_window(void) {
  win = xcb_generate_id(c);
  xcb_create_window(c,                             /* conn */
                    screen->root_depth,            /* depth */
                    win,                           /* wid */
                    screen->root,                  /* parent */
                    x, y,                          /* x, y */
                    width, height,                 /* width, height */
                    10,                            /* border_width */
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, /* _class */
                    screen->root_visual,           /* visual */
                    0,                             /* value_mask */
                    NULL                           /* value_list */
  );

  xcb_atom_t window_type_notification =
      get_atom("_NET_WM_WINDOW_TYPE_NOTIFICATION");
  xcb_change_property(c, XCB_PROP_MODE_REPLACE, win,
                      get_atom("_NET_WM_WINDOW_TYPE"), XCB_ATOM_ATOM, 32,
                      sizeof(xcb_atom_t), &window_type_notification);

  xcb_map_window(c, win);
}

int main(void) {
  c = xcb_connect(NULL, NULL);
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

  create_window();
  xcb_flush(c);

  pause();

  return EXIT_SUCCESS;
}
