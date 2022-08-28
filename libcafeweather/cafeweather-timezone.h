/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* cafeweather-timezone.c - Timezone handling
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

#ifndef __CAFEWEATHER_TIMEZONE_H__
#define __CAFEWEATHER_TIMEZONE_H__

#ifndef CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#error "libcafeweather should only be used if you understand that it's subject to change, and is not supported as a fixed API/ABI or as part of the platform"
#endif

#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CafeWeatherTimezone CafeWeatherTimezone;

GType cafeweather_timezone_get_type (void);
#define CAFEWEATHER_TYPE_TIMEZONE (cafeweather_timezone_get_type ())

const char       *cafeweather_timezone_get_name       (CafeWeatherTimezone *zone);
const char       *cafeweather_timezone_get_tzid       (CafeWeatherTimezone *zone);
int               cafeweather_timezone_get_offset     (CafeWeatherTimezone *zone);
gboolean          cafeweather_timezone_has_dst        (CafeWeatherTimezone *zone);
int               cafeweather_timezone_get_dst_offset (CafeWeatherTimezone *zone);

CafeWeatherTimezone *cafeweather_timezone_ref            (CafeWeatherTimezone *zone);
void              cafeweather_timezone_unref          (CafeWeatherTimezone *zone);

CafeWeatherTimezone *cafeweather_timezone_get_utc        (void);

#ifdef __cplusplus
}
#endif

#endif /* __CAFEWEATHER_TIMEZONE_H__ */
