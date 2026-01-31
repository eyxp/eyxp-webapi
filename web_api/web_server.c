/*
 * web_server
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <io.h>
#include <stdio.h>

#include "web_server.h"
#include "web_api.h"     // für request/response + http_parse_request/http_send_response/app_handle
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;
#define SOCKET_INVALID INVALID_SOCKET
#define socket_close closesocket
#define socket_last_error() WSAGetLastError()

static inline int sockets_init(void) {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
}

static inline void sockets_cleanup(void) {
    WSACleanup();
}

static void log_wsa_err(const char *where) {
    const int e = (int) WSAGetLastError();
    fprintf(stderr, "[ERR] %s failed, WSAGetLastError=%d\n", where, e);
}

#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

typedef int socket_t;
#define SOCKET_INVALID (-1)
#define socket_close close
#define socket_last_error() errno

static inline int sockets_init(void) { return 0; }
static inline void sockets_cleanup(void) {
}

static void log_wsa_err(const char *where) {
    int e = errno;
    fprintf(stderr, "[ERR] %s failed, errno=%d\n", where, e);
}
#endif

void log_sock(const char *tag, socket_t s) {
#ifdef _WIN32
    fprintf(stdout, "[DBG] %s socket=%llu\n", tag, (unsigned long long) s);
#else
    fprintf(stdout, "[DBG] %s socket=%d\n", tag, s);
#endif
    fflush(stdout);
}

socket_t setup_socket() {
#ifdef DEBUG
    fprintf(stdout, "[DBG] starting...\n"); fflush(stdout);
#endif

    if (sockets_init() != 0) {
        log_wsa_err("WSAStartup");
        return 1;
    }

    const socket_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == SOCKET_INVALID) {
        log_wsa_err("socket");
        sockets_cleanup();
        return 1;
    }
    log_sock("created listen", s);


    constexpr int yes = 1;
#ifdef _WIN32
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, (int) sizeof(yes)) < 0) {
        log_wsa_err("setsockopt(SO_REUSEADDR)");
        // not fatal
    }
#else
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        log_wsa_err("setsockopt(SO_REUSEADDR)");
    }
#endif

    return s;
}

socket_t setup_web_server(const socket_t *socket, const int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(*socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        log_wsa_err("bind");
        socket_close(*socket);
        sockets_cleanup();
    }


    if (listen(*socket, 16) < 0) {
        log_wsa_err("listen");
        socket_close(*socket);
        sockets_cleanup();
    }

    fprintf(stdout, "Listening on http://127.0.0.1:%i/\n", port);
    fflush(stdout);

    return *socket;
}

void listen_web_server(eyxp_app_t* app, socket_t *socket) {
    for (;;) {
        fprintf(stdout, "[DBG] waiting for accept...\n"); fflush(stdout);

        const socket_t client = accept(*socket, NULL, NULL);
        if (client == SOCKET_INVALID) {
            log_wsa_err("accept");
            return;
        }
        log_sock("accept", client);

        char buffer[8192];
        const int n = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            if (n < 0) log_wsa_err("recv");

            fprintf(stdout, "[DBG] recv returned %d, closing client\n", n);

            fflush(stdout);
            socket_close(client);
            continue;
        }
        buffer[n] = '\0';

        fprintf(stdout, "[DBG] recv bytes=%d\n", n);
        fflush(stdout);

        eyxp_http_request_t request;
        const int result = http_parse_request(buffer, &request);

        if (result != 0) {
            const char *bad =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Length: 12\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Bad Request\n";

            const int sn = send(client, bad, (int) strlen(bad), 0);
            fprintf(stdout, "[DBG] sent 400 bytes=%d\n", sn);
            if (sn < 0) log_wsa_err("send(400)");
            fflush(stdout);

            socket_close(client);
            continue;
        }

        eyxp_http_response_t response;
        app_handle(app, &request, &response);

        fprintf(stdout, "[DBG] before http_send_response\n");
        fflush(stdout);
        const int res = http_send_response(client, &response);
        fflush(stdout);

        if (res != 0) {
            fprintf(stderr, "[ERR] http_send_response failed, sockerr=%d\n", (int) socket_last_error());
        }
    }
}
