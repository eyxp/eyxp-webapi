/*
* web_routes
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_WEB_ROUTES_H
#define EYXP_WEBAPI_WEB_ROUTES_H

#include <stddef.h>
#include "web_api.h"
#include "web_config.h"

#ifdef __cplusplus
extern "C" {



#endif

#define APPLICATION_JSON "application/json"
#define TEXT_PLAIN_UTF8 "text/plain; charset=utf-8"

typedef struct {
    char name[EYXP_MAX_PARAM_NAME];
} eyxp_route_param_def_t;

typedef struct {
    const char *method;
    char path[256];
    size_t param_count;
    eyxp_route_param_def_t params[EYXP_MAX_ROUTE_PARAMS];
    eyxp_route_handler_t handler;
} eyxp_route_t;

void register_route(eyxp_app_t *app,
                    const char *method,
                    const char *path_pattern,
                    eyxp_route_handler_t handler);

void send_response(eyxp_http_response_t *response,
                   int status,
                   const char *status_text,
                   const char *content_type,
                   const char *body);

void response_text(eyxp_http_response_t *response,
                   int status,
                   const char *status_text,
                   const char *body);

void response_json(eyxp_http_response_t *response,
                   int status,
                   const char *status_text,
                   const char *json_body);

const char *eyxp_param_get(const eyxp_http_request_t *request, const char *name);

#ifdef __cplusplus
}
#endif

#endif
