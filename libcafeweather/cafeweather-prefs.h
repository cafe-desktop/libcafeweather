/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* cafeweather-prefs.h - Preference handling functions
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

#ifndef __CAFEWEATHER_PREFS_H_
#define __CAFEWEATHER_PREFS_H_


#ifndef CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#error "libcafeweather should only be used if you understand that it's subject to change, and is not supported as a fixed API/ABI or as part of the platform"
#endif


#include <libcafeweather/weather.h>
#include <gio/gio.h>

/* gsettings keys */
#define GSETTINGS_TEMP_UNIT     "temperature-unit"
#define GSETTINGS_SPEED_UNIT    "speed-unit"
#define GSETTINGS_PRESSURE_UNIT "pressure-unit"
#define GSETTINGS_DISTANCE_UNIT "distance-unit"

typedef struct _MateWeatherPrefs MateWeatherPrefs;

struct _MateWeatherPrefs {
    WeatherLocation *location;
    gboolean show_notifications;
    gint update_interval;  /* in seconds */
    gboolean update_enabled;
    gboolean detailed;
    gboolean radar_enabled;
    gboolean use_custom_radar_url;
    gchar *radar;

    TempUnit     temperature_unit;
    SpeedUnit    speed_unit;
    PressureUnit pressure_unit;
    DistanceUnit distance_unit;
};

void		cafeweather_prefs_load			(MateWeatherPrefs *prefs,
                                             GSettings *settings);

const char *  cafeweather_prefs_get_temp_display_name    (TempUnit temp);
const char *  cafeweather_prefs_get_speed_display_name    (SpeedUnit speed);
const char *  cafeweather_prefs_get_pressure_display_name  (PressureUnit pressure);
const char *  cafeweather_prefs_get_distance_display_name  (DistanceUnit distance);

#endif /* __CAFEWEATHER_PREFS_H_ */
