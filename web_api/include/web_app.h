/*
 * web_app
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * web_app
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef EYXP_WEBAPI_APP_H
#define EYXP_WEBAPI_APP_H

#include <stddef.h>
#include "web_routes.h"

#ifdef __cplusplus
extern "C" {

#endif

#define EYXP_ROUTE_MAX 64

typedef struct eyxp_app {
    eyxp_route_t routes[EYXP_ROUTE_MAX];
    size_t count;
} eyxp_app_t;

eyxp_app_t eyxp_app_init(void);

#ifdef __cplusplus
}
#endif

#endif // EYXP_WEBAPI_APP_H

