/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* weather-bom.c - Australian Bureau of Meteorology forecast source
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#define CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "weather.h"
#include "weather-priv.h"

static void
bom_finish (GObject *object, GAsyncResult *result, gpointer data)
{
    char *p, *rp;
    SoupSession *session = SOUP_SESSION (object);
    SoupMessage *msg = soup_session_get_async_result_message (session, result);
    WeatherInfo *info = (WeatherInfo *)data;
    GBytes *response_body = NULL;
    const gchar *msgdata;

    g_return_if_fail (info != NULL);

    response_body = soup_session_send_and_read_finish (session, result, NULL);

    if (!SOUP_STATUS_IS_SUCCESSFUL (soup_message_get_status (msg))) {
        g_warning ("Failed to get BOM forecast data: %d %s.\n",
                   soup_message_get_status (msg), soup_message_get_reason_phrase (msg));
        request_done (info, FALSE);
        g_bytes_unref (response_body);
        return;
    }

    msgdata = g_bytes_get_data (response_body, NULL);

    p = strstr (msgdata, "Forecast for the rest");

    if (p != NULL) {
        rp = strstr (p, "The next routine forecast will be issued");
        if (rp == NULL)
            info->forecast = g_strdup (p);
        else
            info->forecast = g_strndup (p, rp - p);
    }

    if (info->forecast == NULL)
        info->forecast = g_strdup (msgdata);

    g_print ("%s\n",  info->forecast);
    request_done (info, TRUE);

    g_bytes_unref (response_body);
}

void
bom_start_open (WeatherInfo *info)
{
    gchar *url;
    SoupMessage *msg;
    WeatherLocation *loc;

    loc = info->location;

    url = g_strdup_printf ("http://www.bom.gov.au/fwo/%s.txt",
			   loc->zone + 1);

    msg = soup_message_new ("GET", url);
    soup_session_send_and_read_async (info->session, msg, G_PRIORITY_DEFAULT, NULL, bom_finish, info);
    g_free (url);

    info->requests_pending++;
}
