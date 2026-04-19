#include "proxy.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *port = DEFAULT_PROXY_PORT;

    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        return 1;
    }

    if (argc >= 2) {
        port = argv[1];
    }

    signal(SIGPIPE, SIG_IGN);

    printf("starting cproxy on port %s\n", port);
    return run_proxy_server(port);
}
