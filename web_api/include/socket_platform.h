/*
* socket_platform
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_SOCKET_PLATFORM_H
#define EYXP_WEBAPI_SOCKET_PLATFORM_H

#ifdef __cplusplus
extern "C" {

#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
typedef int socket_t;
#endif

#ifdef __cplusplus
}
#endif

#endif // EYXP_WEBAPI_SOCKET_PLATFORM_H
