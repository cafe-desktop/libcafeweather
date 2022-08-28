/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* location-entry.h - Location-selecting text entry
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

#ifndef CAFEWEATHER_LOCATION_ENTRY_H
#define CAFEWEATHER_LOCATION_ENTRY_H 1

#include <gtk/gtk.h>
#include <libcafeweather/cafeweather-location.h>

#define CAFEWEATHER_TYPE_LOCATION_ENTRY            (cafeweather_location_entry_get_type ())
#define CAFEWEATHER_LOCATION_ENTRY(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), CAFEWEATHER_TYPE_LOCATION_ENTRY, CafeWeatherLocationEntry))
#define CAFEWEATHER_LOCATION_ENTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CAFEWEATHER_TYPE_LOCATION_ENTRY, CafeWeatherLocationEntryClass))
#define CAFEWEATHER_IS_LOCATION_ENTRY(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), CAFEWEATHER_TYPE_LOCATION_ENTRY))
#define CAFEWEATHER_IS_LOCATION_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CAFEWEATHER_TYPE_LOCATION_ENTRY))
#define CAFEWEATHER_LOCATION_ENTRY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CAFEWEATHER_TYPE_LOCATION_ENTRY, CafeWeatherLocationEntryClass))

typedef struct {
    GtkEntry parent;

    /*< private >*/
    CafeWeatherLocation *location, *top;
    guint custom_text : 1;
} CafeWeatherLocationEntry;

typedef struct {
    GtkEntryClass parent_class;

} CafeWeatherLocationEntryClass;

GType             cafeweather_location_entry_get_type     (void);

GtkWidget        *cafeweather_location_entry_new          (CafeWeatherLocation      *top);

void              cafeweather_location_entry_set_location (CafeWeatherLocationEntry *entry,
							CafeWeatherLocation      *loc);
CafeWeatherLocation *cafeweather_location_entry_get_location (CafeWeatherLocationEntry *entry);

gboolean          cafeweather_location_entry_has_custom_text (CafeWeatherLocationEntry *entry);

gboolean          cafeweather_location_entry_set_city     (CafeWeatherLocationEntry *entry,
							const char            *city_name,
							const char            *code);

#endif
