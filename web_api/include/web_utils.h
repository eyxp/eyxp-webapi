/*
 * web_utils
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_WEB_UTILS_H
#define EYXP_WEBAPI_WEB_UTILS_H
#include <stddef.h>

char* find_header_end(const char* string);
int starts_with_ci(const char* string, const char* prefix);
size_t parse_content_length(const char* header_start);
int send_all(socket_t socket, const char* buffer, size_t length);

#endif //EYXP_WEBAPI_WEB_UTILS_H