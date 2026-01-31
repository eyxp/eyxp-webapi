/*
 * web_utils
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "socket_platform.h"

char* find_header_end(const char* string) {
    return strstr(string, "\r\n\r\n");
}

int start_with_ci(const char* string, const char* prefix) {
    for (; *prefix; string++, prefix++) {
        char a = *string, b = *prefix;
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
        if (a != b) return 0;
    }
    return 1;
}

size_t parse_content_length(const char* header_start) {
    size_t cl = 0;
    const char* line = header_start;
    while (line && *line) {
        char* eol = strstr(line, "\r\n");
        if (!eol) break;
        if (eol == line) break;
        *eol = '\0';

        if (start_with_ci(line, "Content-Length:")) {
            const char* p = line + strlen("Content-Length:");
            while (*p == ' ' || *p == '\t') p++;
            const unsigned long v = strtoul(p, nullptr, 10);
            cl = (size_t)v;
        }

        *eol = '\r';
        line = eol + 2;
    }
    return cl;
}

int send_all(const socket_t socket, const char* buf, size_t length) {
    size_t offset = 0;
    while (offset < length) {
#ifdef _WIN32
        int n = send(socket, buf + offset, (int)(length - offset), 0);
        if (n == SOCKET_ERROR || n == 0) return 1;
#else
        ssize_t n = send(s, buf + off, len - off, 0);
        if (n <= 0) return 1;
#endif
        offset += (size_t)n;
    }
    return 0;
}