#include <argp.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"

static const char doc[] = "allim -- send configurable notifications to allimd";

static const struct argp_option options[] = {
    {"follow mouse", 'f', 0, 0, "Show window on selected monitor"},
    {"monitor", 'm', "MONITOR", 0, "Index of monitor to display window"},
    {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct payload *payload = state->input;

    switch (key) {
        case ARGP_KEY_ARG:
            switch (state->arg_num) {
                case 0:
                    payload->header = arg;
                    break;
                case 1:
                    payload->text = arg;
                    break;
                default:
                    argp_usage(state);
                    break;
            }
            break;
        case 'f':
            payload->config.follow_mouse = 1;
            break;
        case 'm':
            payload->config.monitor = atoi(arg);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2) argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static char args_doc[] = "<HEADER> <TEXT>";

static struct argp argp = {options, parse_opt, args_doc, doc};

static char *marshal(char *header, char *text, struct config *config,
                     size_t payload_size) {
    char *res = malloc(payload_size);
    memcpy(res, config, sizeof(struct config));

    size_t header_size = str_marshal(res + sizeof(struct config), header);
    str_marshal(res + sizeof(struct config) + header_size, text);

    return res;
}

int main(int argc, char **argv) {
    struct payload payload;

    /* Set default values */
    payload.config.follow_mouse = 0;
    payload.config.monitor = 0;

    argp_parse(&argp, argc, argv, 0, 0, &payload);

    size_t payload_size = sizeof(struct config) + (2 * sizeof(size_t)) +
                          strlen(payload.header) + strlen(payload.text) + 2;
    char *marshaled_payload =
        marshal(payload.header, payload.text, &payload.config, payload_size);

    int fd;
    if ((fd = open(PIPE_PATH, O_WRONLY)) < 0) {
        perror("open");
        die("Could not open %s for writing", PIPE_PATH);
    }

    /* Write marshaled payload to pipe */
    write(fd, marshaled_payload, payload_size);

    close(fd);
    unlink(PIPE_PATH);

    return EXIT_SUCCESS;
}
