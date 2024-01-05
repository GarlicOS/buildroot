#ifndef NETWORK_H
#define NETWORK_H

#include <glib.h>
#include <stdbool.h>

#ifdef __linux__
#include <NetworkManager.h>
#endif

/**
 * @brief A single access point.
 */
struct network_access_point
{
    char * ssid;
    char * security;
    char * password;
    bool active;
};

/**
 * @brief Changes the application's access point.
 */
void network_change_access_point(struct network_access_point * access_point);

/**
 * @brief Returns a list of access points.
 * @see https://gitlab.freedesktop.org/NetworkManager/NetworkManager/-/blob/31a12ee/examples/C/glib/get-ap-info-libnm.c
 *
 * The caller must explicitly free the returned item by calling network_free_access_points.
 */
int network_get_access_points(struct network_access_point *** access_points);

/**
 * @brief Frees the given access points.
 */
void network_free_access_points(struct network_access_point ** access_points, int count);

/**
 * @brief Free the given access point.
 */
void network_free_access_point(struct network_access_point * access_point);

#endif
