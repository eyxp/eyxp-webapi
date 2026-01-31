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


#pragma once
#include "web_routes.h"
#include "web_api.h"

#define APPLICATION_JSON "application/json"
#define TEXT_PLAIN_UTF8 "text/plain; charset=utf-8"

typedef void (*eyxp_route_handler_t)(const eyxp_http_request_t* request,
                                     eyxp_http_response_t* response);

typedef struct {
    const char* method;
    const char* path;
    eyxp_route_handler_t route_handler;
} eyxp_route_t;

int register_route(eyxp_app_t* app,
                   const char* method,
                   const char* path,
                   eyxp_route_handler_t route_handler);

void send_response(eyxp_http_response_t* response,
                   int status,
                   const char* status_text,
                   const char* content_type,
                   const char* body);

void response_text(eyxp_http_response_t* response,
                   int status,
                   const char* status_text,
                   const char* body);

void response_json(eyxp_http_response_t* response, int status, const char* status_text, const char* json_body);

#endif //EYXP_WEBAPI_WEB_ROUTES_H

