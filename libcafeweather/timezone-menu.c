/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* timezone-menu.c - Timezone-selecting menu
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "timezone-menu.h"
#include "weather-priv.h"

#include <string.h>

/**
 * SECTION:timezone-menu
 * @Title: CafeWeatherTimezoneMenu
 *
 * A #CtkComboBox subclass for choosing a #CafeWeatherTimezone
 */

G_DEFINE_TYPE (CafeWeatherTimezoneMenu, cafeweather_timezone_menu, CTK_TYPE_COMBO_BOX)

enum {
    PROP_0,

    PROP_TOP,
    PROP_TZID,

    LAST_PROP
};

static void set_property (GObject *object, guint prop_id,
			  const GValue *value, GParamSpec *pspec);
static void get_property (GObject *object, guint prop_id,
			  GValue *value, GParamSpec *pspec);

static void changed      (CtkComboBox *combo);

static CtkTreeModel *cafeweather_timezone_model_new (CafeWeatherLocation *top);
static gboolean row_separator_func (CtkTreeModel *model, CtkTreeIter *iter,
				    gpointer data);
static void is_sensitive (CtkCellLayout *cell_layout, CtkCellRenderer *cell,
			  CtkTreeModel *tree_model, CtkTreeIter *iter, gpointer data);

static void
cafeweather_timezone_menu_init (CafeWeatherTimezoneMenu *menu)
{
    CtkCellRenderer *renderer;

    ctk_combo_box_set_row_separator_func (CTK_COMBO_BOX (menu),
					  row_separator_func, NULL, NULL);

    renderer = ctk_cell_renderer_text_new ();
    ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (menu), renderer, TRUE);
    ctk_cell_layout_set_attributes (CTK_CELL_LAYOUT (menu), renderer,
				    "markup", 0,
				    NULL);
    ctk_cell_layout_set_cell_data_func (CTK_CELL_LAYOUT (menu),
					renderer, is_sensitive, NULL, NULL);
}

static void
finalize (GObject *object)
{
    CafeWeatherTimezoneMenu *menu = CAFEWEATHER_TIMEZONE_MENU (object);

    if (menu->zone)
	cafeweather_timezone_unref (menu->zone);

    G_OBJECT_CLASS (cafeweather_timezone_menu_parent_class)->finalize (object);
}

static void
cafeweather_timezone_menu_class_init (CafeWeatherTimezoneMenuClass *timezone_menu_class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (timezone_menu_class);
    CtkComboBoxClass *combo_class = CTK_COMBO_BOX_CLASS (timezone_menu_class);

    object_class->finalize = finalize;
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    combo_class->changed = changed;

    /* properties */
    g_object_class_install_property (
	object_class, PROP_TOP,
	g_param_spec_pointer ("top",
			      "Top Location",
			      "The CafeWeatherLocation whose children will be used to fill in the menu",
			      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (
	object_class, PROP_TZID,
	g_param_spec_string ("tzid",
			     "TZID",
			     "The selected TZID",
			     NULL,
			     G_PARAM_READWRITE));
}

static void
set_property (GObject *object, guint prop_id,
	      const GValue *value, GParamSpec *pspec)
{
    CtkTreeModel *model;

    switch (prop_id) {
    case PROP_TOP:
	model = cafeweather_timezone_model_new (g_value_get_pointer (value));
	ctk_combo_box_set_model (CTK_COMBO_BOX (object), model);
	g_object_unref (model);
	ctk_combo_box_set_active (CTK_COMBO_BOX (object), 0);
	break;

    case PROP_TZID:
	cafeweather_timezone_menu_set_tzid (CAFEWEATHER_TIMEZONE_MENU (object),
					 g_value_get_string (value));
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void
get_property (GObject *object, guint prop_id,
	      GValue *value, GParamSpec *pspec)
{
    CafeWeatherTimezoneMenu *menu = CAFEWEATHER_TIMEZONE_MENU (object);

    switch (prop_id) {
    case PROP_TZID:
	g_value_set_string (value, cafeweather_timezone_menu_get_tzid (menu));
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

enum {
    CAFEWEATHER_TIMEZONE_MENU_NAME,
    CAFEWEATHER_TIMEZONE_MENU_ZONE
};

static void
changed (CtkComboBox *combo)
{
    CafeWeatherTimezoneMenu *menu = CAFEWEATHER_TIMEZONE_MENU (combo);
    CtkTreeIter iter;

    if (menu->zone)
	cafeweather_timezone_unref (menu->zone);

    ctk_combo_box_get_active_iter (combo, &iter);
    ctk_tree_model_get (ctk_combo_box_get_model (combo), &iter,
			CAFEWEATHER_TIMEZONE_MENU_ZONE, &menu->zone,
			-1);

    if (menu->zone)
	cafeweather_timezone_ref (menu->zone);

    g_object_notify (G_OBJECT (combo), "tzid");
}

static void
append_offset (GString *desc, int offset)
{
    int hours, minutes;

    hours = offset / 60;
    minutes = (offset > 0) ? offset % 60 : -offset % 60;

    if (minutes)
	g_string_append_printf (desc, "GMT%+d:%02d", hours, minutes);
    else if (hours)
	g_string_append_printf (desc, "GMT%+d", hours);
    else
	g_string_append (desc, "GMT");
}

static char *
get_offset (CafeWeatherTimezone *zone)
{
    GString *desc;

    desc = g_string_new (NULL);
    append_offset (desc, cafeweather_timezone_get_offset (zone));
    if (cafeweather_timezone_has_dst (zone)) {
	g_string_append (desc, " / ");
	append_offset (desc, cafeweather_timezone_get_dst_offset (zone));
    }
    return g_string_free (desc, FALSE);
}

static void
insert_location (CtkTreeStore *store, CafeWeatherTimezone *zone, const char *loc_name, CtkTreeIter *parent)
{
    CtkTreeIter iter;
    char *name, *offset;

    offset = get_offset (zone);
    name = g_strdup_printf ("%s <small>(%s)</small>",
                            loc_name ? loc_name : cafeweather_timezone_get_name (zone),
                            offset);
    ctk_tree_store_append (store, &iter, parent);
    ctk_tree_store_set (store, &iter,
                        CAFEWEATHER_TIMEZONE_MENU_NAME, name,
                        CAFEWEATHER_TIMEZONE_MENU_ZONE, cafeweather_timezone_ref (zone),
                        -1);
    g_free (name);
    g_free (offset);
}

static void
insert_locations (CtkTreeStore *store, CafeWeatherLocation *loc)
{
    int i;

    if (cafeweather_location_get_level (loc) < CAFEWEATHER_LOCATION_COUNTRY) {
	CafeWeatherLocation **children;

	children = cafeweather_location_get_children (loc);
	for (i = 0; children[i]; i++)
	    insert_locations (store, children[i]);
	cafeweather_location_free_children (loc, children);
    } else {
	CafeWeatherTimezone **zones;
	CtkTreeIter iter;

	zones = cafeweather_location_get_timezones (loc);
	if (zones[1]) {
	    ctk_tree_store_append (store, &iter, NULL);
	    ctk_tree_store_set (store, &iter,
				CAFEWEATHER_TIMEZONE_MENU_NAME, cafeweather_location_get_name (loc),
				-1);

	    for (i = 0; zones[i]; i++) {
                insert_location (store, zones[i], NULL, &iter);
	    }
	} else if (zones[0]) {
            insert_location (store, zones[0], cafeweather_location_get_name (loc), NULL);
	}

	cafeweather_location_free_timezones (loc, zones);
    }
}

static CtkTreeModel *
cafeweather_timezone_model_new (CafeWeatherLocation *top)
{
    CtkTreeStore *store;
    CtkTreeModel *model;
    CtkTreeIter iter;
    char *unknown;
    CafeWeatherTimezone *utc;

    store = ctk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
    model = CTK_TREE_MODEL (store);

    unknown = g_markup_printf_escaped ("<i>%s</i>", C_("timezone", "Unknown"));

    ctk_tree_store_append (store, &iter, NULL);
    ctk_tree_store_set (store, &iter,
			CAFEWEATHER_TIMEZONE_MENU_NAME, unknown,
			CAFEWEATHER_TIMEZONE_MENU_ZONE, NULL,
			-1);

    utc = cafeweather_timezone_get_utc ();
    if (utc) {
        insert_location (store, utc, NULL, NULL);
        cafeweather_timezone_unref (utc);
    }

    ctk_tree_store_append (store, &iter, NULL);

    g_free (unknown);

    insert_locations (store, top);

    return model;
}

static gboolean
row_separator_func (CtkTreeModel *model, CtkTreeIter *iter, gpointer data)
{
    char *name;

    ctk_tree_model_get (model, iter,
			CAFEWEATHER_TIMEZONE_MENU_NAME, &name,
			-1);
    if (name) {
	g_free (name);
	return FALSE;
    } else
	return TRUE;
}

static void
is_sensitive (CtkCellLayout *cell_layout, CtkCellRenderer *cell,
	      CtkTreeModel *tree_model, CtkTreeIter *iter, gpointer data)
{
    gboolean sensitive;

    sensitive = !ctk_tree_model_iter_has_child (tree_model, iter);
    g_object_set (cell, "sensitive", sensitive, NULL);
}

/**
 * cafeweather_timezone_menu_new:
 * @top: the top-level location for the menu.
 *
 * Creates a new #CafeWeatherTimezoneMenu.
 *
 * @top will normally be a location returned from
 * cafeweather_location_new_world(), but you can create a menu that
 * contains the timezones from a smaller set of locations if you want.
 *
 * Return value: the new #CafeWeatherTimezoneMenu
 **/
CtkWidget *
cafeweather_timezone_menu_new (CafeWeatherLocation *top)
{
    return g_object_new (CAFEWEATHER_TYPE_TIMEZONE_MENU,
			 "top", top,
			 NULL);
}

typedef struct {
    CtkComboBox *combo;
    const char  *tzid;
} SetTimezoneData;

static gboolean
check_tzid (CtkTreeModel *model, CtkTreePath *path,
	    CtkTreeIter *iter, gpointer data)
{
    SetTimezoneData *tzd = data;
    CafeWeatherTimezone *zone;

    ctk_tree_model_get (model, iter,
			CAFEWEATHER_TIMEZONE_MENU_ZONE, &zone,
			-1);
    if (!zone)
	return FALSE;

    if (!strcmp (cafeweather_timezone_get_tzid (zone), tzd->tzid)) {
	ctk_combo_box_set_active_iter (tzd->combo, iter);
	return TRUE;
    } else
	return FALSE;
}

/**
 * cafeweather_timezone_menu_set_tzid:
 * @menu: a #CafeWeatherTimezoneMenu
 * @tzid: (allow-none): a tzdata id (eg, "America/New_York")
 *
 * Sets @menu to the given @tzid. If @tzid is %NULL, sets @menu to
 * "Unknown".
 **/
void
cafeweather_timezone_menu_set_tzid (CafeWeatherTimezoneMenu *menu,
				 const char           *tzid)
{
    SetTimezoneData tzd;

    g_return_if_fail (CAFEWEATHER_IS_TIMEZONE_MENU (menu));

    if (!tzid) {
	ctk_combo_box_set_active (CTK_COMBO_BOX (menu), 0);
	return;
    }

    tzd.combo = CTK_COMBO_BOX (menu);
    tzd.tzid = tzid;
    ctk_tree_model_foreach (ctk_combo_box_get_model (tzd.combo),
			    check_tzid, &tzd);
}

/**
 * cafeweather_timezone_menu_get_tzid:
 * @menu: a #CafeWeatherTimezoneMenu
 *
 * Gets @menu's timezone id.
 *
 * Return value: (allow-none): @menu's tzid, or %NULL if no timezone
 * is selected.
 **/
const char *
cafeweather_timezone_menu_get_tzid (CafeWeatherTimezoneMenu *menu)
{
    g_return_val_if_fail (CAFEWEATHER_IS_TIMEZONE_MENU (menu), NULL);

    if (!menu->zone)
	return NULL;
    return cafeweather_timezone_get_tzid (menu->zone);
}

