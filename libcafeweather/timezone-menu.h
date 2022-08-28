/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* timezone-menu.h - Timezone-selecting menu
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

#ifndef CAFEWEATHER_TIMEZONE_MENU_H
#define CAFEWEATHER_TIMEZONE_MENU_H 1

#include <gtk/gtk.h>
#include <libcafeweather/cafeweather-location.h>

#define CAFEWEATHER_TYPE_TIMEZONE_MENU            (cafeweather_timezone_menu_get_type ())
#define CAFEWEATHER_TIMEZONE_MENU(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), CAFEWEATHER_TYPE_TIMEZONE_MENU, CafeWeatherTimezoneMenu))
#define CAFEWEATHER_TIMEZONE_MENU_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CAFEWEATHER_TYPE_TIMEZONE_MENU, CafeWeatherTimezoneMenuClass))
#define CAFEWEATHER_IS_TIMEZONE_MENU(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), CAFEWEATHER_TYPE_TIMEZONE_MENU))
#define CAFEWEATHER_IS_TIMEZONE_MENU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CAFEWEATHER_TYPE_TIMEZONE_MENU))
#define CAFEWEATHER_TIMEZONE_MENU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CAFEWEATHER_TYPE_TIMEZONE_MENU, CafeWeatherTimezoneMenuClass))

typedef struct {
    GtkComboBox parent;

    /*< private >*/
    CafeWeatherTimezone *zone;
} CafeWeatherTimezoneMenu;

typedef struct {
    GtkComboBoxClass parent_class;

} CafeWeatherTimezoneMenuClass;

GType       cafeweather_timezone_menu_get_type         (void);

GtkWidget  *cafeweather_timezone_menu_new              (CafeWeatherLocation     *top);

void        cafeweather_timezone_menu_set_tzid         (CafeWeatherTimezoneMenu *menu,
						     const char           *tzid);
const char *cafeweather_timezone_menu_get_tzid         (CafeWeatherTimezoneMenu *menu);

#endif
