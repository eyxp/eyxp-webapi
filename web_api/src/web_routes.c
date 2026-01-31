/*
 * web_routes
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "web_app.h"
#include "web_routes.h"

#include <stdio.h>
#include <string.h>

void register_route(eyxp_app_t *app,
                    const char *method,
                    const char *path_pattern,
                    eyxp_route_handler_t handler) {
    if (!app || !method || !path_pattern || !handler) return;
    if (app->count >= EYXP_ROUTE_MAX) return;

    eyxp_route_t *r = &app->routes[app->count++];
    memset(r, 0, sizeof(*r));

    r->method = method;
    strncpy(r->path, path_pattern, sizeof(r->path) - 1);
    r->path[sizeof(r->path) - 1] = '\0';
    r->handler = handler;

    const char *pos = path_pattern;
    while (*pos == '/') pos++;
    while (*pos && r->param_count < EYXP_MAX_ROUTE_PARAMS) {
        if (*pos == ':') {
            const char *name_start = pos + 1;
            const char *name_end = strchr(name_start, '/');
            if (!name_end) name_end = name_start + strlen(name_start);

            eyxp_route_param_def_t *pd = &r->params[r->param_count++];
            size_t len = (size_t) (name_end - name_start);
            if (len >= EYXP_MAX_PARAM_NAME) len = EYXP_MAX_PARAM_NAME - 1;
            memcpy(pd->name, name_start, len);
            pd->name[len] = '\0';
        }
        pos = strchr(pos, '/');
        if (!pos) break;
        pos++;
        while (*pos == '/') pos++;
    }
}


void send_response(eyxp_http_response_t *response,
                   const int status,
                   const char *status_text,
                   const char *content_type,
                   const char *body) {
    if (!response) return;

    memset(response, 0, sizeof(*response));

    response->status = status;
    response->status_text = status_text;
    response->content_type = content_type;
    response->body = body;
    response->body_length = body ? strlen(body) : 0;
}

void response_text(eyxp_http_response_t *response,
                   const int status,
                   const char *status_text,
                   const char *body) {
    send_response(response, status, status_text, TEXT_PLAIN_UTF8, body);
}

void response_json(eyxp_http_response_t *response, const int status, const char *status_text, const char *json_body) {
    send_response(response, status, status_text, status_text, json_body);
}

const char *eyxp_param_get(const eyxp_http_request_t *request, const char *name) {
    for (size_t i = 0; i < request->param_count; ++i) {
        if (strcmp(request->params[i].name, name) == 0)
            return request->params[i].value;
    }
    return NULL;
}
