/*
 * web_api
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_WEB_API_H
#define EYXP_WEBAPI_WEB_API_H

#include <stddef.h>
#include "socket_platform.h"
#include "web_config.h"

#ifdef __cplusplus
extern "C" {


#endif

typedef struct eyxp_app eyxp_app_t;

typedef struct {
    char name[EYXP_MAX_PARAM_NAME];
    char value[EYXP_MAX_PARAM_VALUE];
} eyxp_path_param_t;

typedef struct {
    char method[8];
    char path[256];
    size_t content_length;
    const char *body;
    size_t body_length;
    eyxp_path_param_t params[EYXP_MAX_ROUTE_PARAMS];
    size_t param_count;
} eyxp_http_request_t;

typedef struct {
    int status;
    const char *status_text;
    const char *content_type;
    const char *body;
    size_t body_length;
} eyxp_http_response_t;

typedef void (*eyxp_route_handler_t)(const eyxp_http_request_t *request, eyxp_http_response_t *response);

int http_parse_request(const char *request_buffer, eyxp_http_request_t *output_request);

void app_handle(const eyxp_app_t *app, const eyxp_http_request_t *request, eyxp_http_response_t *response);

int http_send_response(socket_t client_socket, const eyxp_http_response_t *response);

const char *eyxp_param_get(const eyxp_http_request_t *request, const char *name);

#ifdef __cplusplus
}
#endif

#endif
