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
#include "web_api.h"    
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

void listen_web_server(eyxp_app_t *app, socket_t *socket) {
    fflush(stdout);

    if (listen(*socket, 16) < 0) {
        log_wsa_err("listen");
        return;
    }

    fflush(stdout);

    unsigned long mode = 1;
    ioctlsocket(*socket, FIONBIO, &mode);

    while (1) {
        fflush(stdout);

        socket_t client = accept(*socket, NULL, NULL);
        int err = socket_last_error();
        fflush(stdout);

        if (client == INVALID_SOCKET) {
            if (err != WSAEWOULDBLOCK && err != EWOULDBLOCK) {
                log_wsa_err("accept");
            }
            Sleep(10); // CPU schonen
            continue;
        }

        fflush(stdout);

        char recv_buf[2048] = {0};
        const int recv_len = recv(client, recv_buf, sizeof(recv_buf) - 1, 0);
        fflush(stdout);

        if (recv_len <= 0) {
            closesocket(client);
            continue;
        }

        fflush(stdout);

        eyxp_http_request_t req = {0};
        eyxp_http_response_t res = {0};

        if (http_parse_request(recv_buf, &req) == 0) {
            fflush(stdout);
            app_handle(app, &req, &res);
        } else {
            response_text(&res, 400, "Bad Request", "Parse error\n");
        }

        fflush(stdout);
        http_send_response(client, &res);

        closesocket(client);
        fflush(stdout);
    }
}
