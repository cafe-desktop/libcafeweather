/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* cafeweather-xml.h
 *
 * Copyright (C) 2004 Gareth Owen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __CAFEWEATHER_XML_H__
#define __CAFEWEATHER_XML_H__

#include <gtk/gtk.h>
#include <libcafeweather/weather.h>

enum
{
    CAFEWEATHER_XML_COL_LOC = 0,
    CAFEWEATHER_XML_COL_POINTER,
    CAFEWEATHER_XML_NUM_COLUMNS
};

GtkTreeModel *cafeweather_xml_load_locations (void);
void          cafeweather_xml_free_locations (GtkTreeModel *locations);

#endif /* __CAFEWEATHER_XML_H__ */
