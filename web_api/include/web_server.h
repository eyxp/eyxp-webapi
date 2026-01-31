/*
 * web_server
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_WEB_SERVER_H
#define EYXP_WEBAPI_WEB_SERVER_H

#pragma once

#include "socket_platform.h"
#include "web_app.h"

socket_t setup_socket(void);
socket_t setup_web_server(const socket_t *socket, int port);
void listen_web_server(eyxp_app_t* app, socket_t* socket);

#endif //EYXP_WEBAPI_WEB_SERVER_H