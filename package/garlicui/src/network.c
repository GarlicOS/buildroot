#ifndef __linux__
#include <glib-object.h>
#endif

#include "network.h"

/**
 * @brief Changes the application's wifi access point.
 */
void network_change_access_point(struct network_access_point * access_point)
{
    // TODO connect to the access point
    // https://networkmanager.dev/docs/libnm/latest/NMClient.html#nm-client-activate-connection-async
    // https://networkmanager.dev/docs/libnm/latest/NMClient.html#nm-client-activate-connection-finish
    // https://networkmanager.dev/docs/libnm/latest/NMClient.html#nm-client-add-and-activate-connection-async
    // https://networkmanager.dev/docs/libnm/latest/NMClient.html#nm-client-add-and-activate-connection-finish
}

/**
 * @brief Returns a list of wifi devices.
 * @see https://gitlab.freedesktop.org/NetworkManager/NetworkManager/-/blob/31a12ee/examples/C/glib/get-ap-info-libnm.c
 *
 * The caller must explicitly free the returned item by calling network_free_access_points.
 */
int network_get_access_points(struct network_access_point *** access_points)
{
    int access_point_count = 0;

    *access_points = NULL;

#ifdef __linux__
    GError * error = NULL;
    NMClient * client = nm_client_new(NULL, &error);

    if (!client)
    {
        goto free_error;
    }

    const GPtrArray * devices = nm_client_get_devices(client);
    NMDevice * device = NULL;

    for (int i = 0, n = devices->len; i < n; i++)
    {
        NMDevice * current_device = g_ptr_array_index(devices, i);

        if (!NM_IS_DEVICE_WIFI(current_device))
        {
            continue;
        }

        device = current_device;
        break;
    }

    if (device == NULL)
    {
        goto free_client;
    }

    NMAccessPoint * active_ap = nm_device_get_state(device) == NM_DEVICE_STATE_ACTIVATED ? nm_device_wifi_get_active_access_point(NM_DEVICE_WIFI(device)) : NULL;
    const GPtrArray * aps = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(device));
#else
    GPtrArray * aps = g_ptr_array_new_full(2, g_object_unref);
    g_ptr_array_add(aps, "Dummy");
#endif

    for (int i = 0, n = aps->len; i < n; i++)
    {
#ifdef __linux__
        NMAccessPoint * ap = g_ptr_array_index(aps, i);
        GBytes * ssid_bytes = nm_access_point_get_ssid(ap);
        char * ssid = nm_utils_ssid_to_utf8(g_bytes_get_data(ssid_bytes, NULL), g_bytes_get_size(ssid_bytes));
        guint32 flags = nm_access_point_get_flags(ap);
        guint32 wpa_flags = nm_access_point_get_wpa_flags(ap);
        guint32 rsn_flags = nm_access_point_get_rsn_flags(ap);
        char * security = "None";
        bool active = active_ap == ap;

        if ((flags & NM_802_11_AP_FLAGS_PRIVACY) && (wpa_flags == NM_802_11_AP_SEC_NONE) && (rsn_flags == NM_802_11_AP_SEC_NONE))
        {
            security = "WEP";
        }

        if (wpa_flags != NM_802_11_AP_SEC_NONE)
        {
            security = "WPA";
        }

        if ((rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK) || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X))
        {
            security = "WPA2";
        }

        if (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_SAE)
        {
            security = "WPA3";
        }
#else
        char * ssid = g_ptr_array_index(aps, i);
        char * security = "WPA2";
        bool active = i == 0;
#endif

        // Allocate memory for the access point
        struct network_access_point * access_point = (struct network_access_point *)calloc(1, sizeof(struct network_access_point));

        // We managed to allocate the memory for the access point
        if (access_point != NULL)
        {
            // Populate the access point
            access_point->ssid = ssid;
            access_point->active = active;
            access_point->security = security;

            // Resize the access points list to accommodate one more access point
            void * resized_access_points = realloc(*access_points, sizeof(struct network_access_point *) * (access_point_count + 1));

            // We managed to resize the playlist access points list
            if (resized_access_points)
            {
                // Override the old pointer (some libc implementations can relocate the buffer)
                *access_points = resized_access_points;

                // Add the populated playlist item to the list
                (*access_points)[access_point_count++] = access_point;
            }
        }
#ifdef __linux__
        g_free(ssid);
#endif
    }
#ifdef __linux__
free_client:
    g_object_unref(client);

free_error:
    g_error_free(error);
#endif

    return access_point_count;
}

/**
 * @brief Frees the given wifi access points.
 */
void network_free_access_points(struct network_access_point ** access_points, int count)
{
    // We've been given valid parameters
    if (access_points != NULL && count != 0)
    {
        // Iterate all access points
        for (int i = 0; i < count; i++)
        {
            // Free the access point
            network_free_access_point(access_points[i]);
        }

        // Free the list itself
        free(access_points);
    }
}

/**
 * @brief Free the given wifi access point.
 */
void network_free_access_point(struct network_access_point * access_point)
{
    // We've been given a seemingly valid access point
    if (access_point != NULL)
    {
        // Free the access point itself
        free(access_point);
    }
}
