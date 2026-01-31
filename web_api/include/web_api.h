/*
 * web_api
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_LIBRARY_H
#define EYXP_WEBAPI_LIBRARY_H

#pragma once

#include "socket_platform.h"

typedef struct eyxp_app eyxp_app_t;

typedef struct {
    char method[8];
    char path[256];
    size_t content_length;
    const char* body;
    size_t body_length;
} eyxp_http_request_t;

typedef struct {
    int status;
    const char* status_text;
    const char* content_type;
    const char* body;
    size_t body_length;
} eyxp_http_response_t;

int http_parse_request(const char* buffer, eyxp_http_request_t* output);
void app_handle(const eyxp_app_t* app, const eyxp_http_request_t* request, eyxp_http_response_t* response);
int http_send_response(socket_t client_fd, const eyxp_http_response_t* response);

#endif //EYXP_WEBAPI_LIBRARY_H