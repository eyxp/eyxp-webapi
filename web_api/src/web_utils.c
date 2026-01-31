/*
 * web_utils
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "web_utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

const char *find_header_end(const char *buffer) {
    if (!buffer) return NULL;
    return strstr(buffer, "\r\n\r\n");
}

int starts_with_ci(const char *string, const char *prefix) {
    if (!string || !prefix) return 0;

    for (; *prefix; ++string, ++prefix) {
        unsigned char a = (unsigned char) *string;
        unsigned char b = (unsigned char) *prefix;

        if (a >= 'A' && a <= 'Z') a = (unsigned char) (a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (unsigned char) (b - 'A' + 'a');

        if (a != b) return 0;
    }
    return 1;
}

size_t parse_content_length(const char *header_start) {
    if (!header_start) return 0;

    const char *p = header_start;
    const char *end = find_header_end(header_start);
    if (!end) {
        end = header_start + strlen(header_start);
    }

    while (p < end) {
        const char *eol = strstr(p, "\r\n");
        if (!eol || eol > end) eol = end;
        if (eol == p) break;

        if ((size_t) (eol - p) >= strlen("Content-Length:") &&
            starts_with_ci(p, "Content-Length:")) {
            const char *v = p + strlen("Content-Length:");
            while (v < eol && (*v == ' ' || *v == '\t')) ++v;

            char tmp[32];
            size_t n = (size_t) (eol - v);
            if (n >= sizeof(tmp)) n = sizeof(tmp) - 1;
            memcpy(tmp, v, n);
            tmp[n] = '\0';

            char *endptr = NULL;
            unsigned long ul = strtoul(tmp, &endptr, 10);
            if (endptr != tmp) {
                return (size_t) ul;
            }
        }

        p = (eol == end) ? end : (eol + 2);
    }

    return 0;
}

int send_all(socket_t socket, const void *buffer, size_t length) {
    const char *buf = (const char *) buffer;
    size_t offset = 0;

    while (offset < length) {
#ifdef _WIN32
        int n = send(socket, buf + offset, (int) (length - offset), 0);
        if (n == SOCKET_ERROR) return 1;
        if (n == 0) return 1;
#else
        ssize_t n = send(socket, buf + offset, length - offset, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return 1;
        }
        if (n == 0) return 1;
#endif
        offset += (size_t) n;
    }

    return 0;
}
