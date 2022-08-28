/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* cafeweather-location.h - Location-handling code
 *
 * Copyright 2008, Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __CAFEWEATHER_LOCATIONS_H__
#define __CAFEWEATHER_LOCATIONS_H__

#ifndef CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#error "libcafeweather should only be used if you understand that it's subject to change, and is not supported as a fixed API/ABI or as part of the platform"
#endif

#include <glib.h>
#include <libcafeweather/cafeweather-timezone.h>
#include <libcafeweather/weather.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MateWeatherLocation MateWeatherLocation;

typedef enum { /*< underscore_name=cafeweather_location_level >*/
    CAFEWEATHER_LOCATION_WORLD,
    CAFEWEATHER_LOCATION_REGION,
    CAFEWEATHER_LOCATION_COUNTRY,
    /* ADM1 = first-order administrative division = state/province, etc */
    CAFEWEATHER_LOCATION_ADM1,
    /* ADM2 = second-order division = county, etc */
    CAFEWEATHER_LOCATION_ADM2,
    CAFEWEATHER_LOCATION_CITY,
    CAFEWEATHER_LOCATION_WEATHER_STATION
} MateWeatherLocationLevel;

GType cafeweather_location_get_type (void);
#define CAFEWEATHER_TYPE_LOCATION (cafeweather_location_get_type ())

MateWeatherLocation      *cafeweather_location_new_world      (gboolean           use_regions);
MateWeatherLocation      *cafeweather_location_ref            (MateWeatherLocation  *loc);
void                   cafeweather_location_unref          (MateWeatherLocation  *loc);

const char            *cafeweather_location_get_name       (MateWeatherLocation  *loc);
const char            *cafeweather_location_get_sort_name  (MateWeatherLocation  *loc);
MateWeatherLocationLevel  cafeweather_location_get_level      (MateWeatherLocation  *loc);
MateWeatherLocation      *cafeweather_location_get_parent     (MateWeatherLocation  *loc);

MateWeatherLocation     **cafeweather_location_get_children   (MateWeatherLocation  *loc);
void                   cafeweather_location_free_children  (MateWeatherLocation  *loc,
							 MateWeatherLocation **children);

gboolean               cafeweather_location_has_coords     (MateWeatherLocation  *loc);
void                   cafeweather_location_get_coords     (MateWeatherLocation  *loc,
							 double            *latitude,
							 double            *longitude);
double                 cafeweather_location_get_distance   (MateWeatherLocation  *loc,
							 MateWeatherLocation  *loc2);

const char            *cafeweather_location_get_country    (MateWeatherLocation  *loc);

MateWeatherTimezone      *cafeweather_location_get_timezone   (MateWeatherLocation  *loc);
MateWeatherTimezone     **cafeweather_location_get_timezones  (MateWeatherLocation  *loc);
void                   cafeweather_location_free_timezones (MateWeatherLocation  *loc,
							 MateWeatherTimezone **zones);

const char            *cafeweather_location_get_code       (MateWeatherLocation  *loc);
char                  *cafeweather_location_get_city_name  (MateWeatherLocation  *loc);

WeatherInfo           *cafeweather_location_get_weather    (MateWeatherLocation  *loc);

#ifdef __cplusplus
}
#endif

#endif /* __CAFEWEATHER_LOCATIONS_H__ */
