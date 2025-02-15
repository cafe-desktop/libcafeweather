/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* cafeweather-xml.c - Locations.xml parsing code
 *
 * Copyright (C) 2005 Ryan Lortie, 2004 Gareth Owen
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
#include <math.h>
#include <locale.h>
#include <ctk/ctk.h>
#include <libxml/xmlreader.h>

#define CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "cafeweather-xml.h"
#include "weather-priv.h"

/**
 * SECTION:cafeweather-xml
 * @Title: cafeweather-xml
 */

static gboolean
cafeweather_xml_parse_node (CafeWeatherLocation *gloc,
			 CtkTreeStore *store, CtkTreeIter *parent)
{
    CtkTreeIter iter, *self = &iter;
    CafeWeatherLocation **children, *parent_loc;
    CafeWeatherLocationLevel level;
    WeatherLocation *wloc;
    const char *name;
    int i;

    name = cafeweather_location_get_name (gloc);
    children = cafeweather_location_get_children (gloc);
    level = cafeweather_location_get_level (gloc);

    if (!children[0] && level < CAFEWEATHER_LOCATION_WEATHER_STATION) {
	cafeweather_location_free_children (gloc, children);
	return TRUE;
    }

    switch (cafeweather_location_get_level (gloc)) {
    case CAFEWEATHER_LOCATION_WORLD:
    case CAFEWEATHER_LOCATION_ADM2:
	self = parent;
	break;

    case CAFEWEATHER_LOCATION_REGION:
    case CAFEWEATHER_LOCATION_COUNTRY:
    case CAFEWEATHER_LOCATION_ADM1:
	/* Create a row with a name but no WeatherLocation */
	ctk_tree_store_append (store, &iter, parent);
	ctk_tree_store_set (store, &iter,
			    CAFEWEATHER_XML_COL_LOC, name,
			    -1);
	break;

    case CAFEWEATHER_LOCATION_CITY:
	/* If multiple children, treat this like a
	 * region/country/adm1. If a single child, merge with that
	 * location.
	 */
	ctk_tree_store_append (store, &iter, parent);
	ctk_tree_store_set (store, &iter,
			    CAFEWEATHER_XML_COL_LOC, name,
			    -1);
	if (children[0] && !children[1]) {
	    wloc = cafeweather_location_to_weather_location (children[0], name);
	    ctk_tree_store_set (store, &iter,
				CAFEWEATHER_XML_COL_POINTER, wloc,
				-1);
	}
	break;

    case CAFEWEATHER_LOCATION_WEATHER_STATION:
	ctk_tree_store_append (store, &iter, parent);
	ctk_tree_store_set (store, &iter,
			    CAFEWEATHER_XML_COL_LOC, name,
			    -1);

	parent_loc = cafeweather_location_get_parent (gloc);
	if (parent_loc && cafeweather_location_get_level (parent_loc) == CAFEWEATHER_LOCATION_CITY)
	    name = cafeweather_location_get_name (parent_loc);
	wloc = cafeweather_location_to_weather_location (gloc, name);
	ctk_tree_store_set (store, &iter,
			    CAFEWEATHER_XML_COL_POINTER, wloc,
			    -1);
	break;
    }

    for (i = 0; children[i]; i++) {
	if (!cafeweather_xml_parse_node (children[i], store, self)) {
	    cafeweather_location_free_children (gloc, children);
	    return FALSE;
	}
    }

    cafeweather_location_free_children (gloc, children);
    return TRUE;
}

CtkTreeModel *
cafeweather_xml_load_locations (void)
{
    CafeWeatherLocation *world;
    CtkTreeStore *store;

    world = cafeweather_location_new_world (TRUE);
    if (!world)
	return NULL;

    store = ctk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);

    if (!cafeweather_xml_parse_node (world, store, NULL)) {
	cafeweather_xml_free_locations ((CtkTreeModel *)store);
	store = NULL;
    }

    cafeweather_location_unref (world);

    return (CtkTreeModel *)store;
}

static gboolean
free_locations (CtkTreeModel *model,
		CtkTreePath  *path G_GNUC_UNUSED,
		CtkTreeIter  *iter,
		gpointer      data G_GNUC_UNUSED)
{
	WeatherLocation *loc = NULL;

	ctk_tree_model_get (model, iter,
			    CAFEWEATHER_XML_COL_POINTER, &loc,
			    -1);

	if (loc) {
		weather_location_free (loc);
		ctk_tree_store_set ((CtkTreeStore *)model, iter,
			    CAFEWEATHER_XML_COL_POINTER, NULL,
			    -1);
	}

	return FALSE;
}

/* Frees model returned from @cafeweather_xml_load_locations. It contains allocated
   WeatherLocation-s, thus this takes care of the freeing of that memory. */
void
cafeweather_xml_free_locations (CtkTreeModel *locations)
{
	if (locations && CTK_IS_TREE_STORE (locations)) {
		ctk_tree_model_foreach (locations, free_locations, NULL);
		g_object_unref (locations);
	}
}
