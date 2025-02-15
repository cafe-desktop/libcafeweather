/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* location-entry.c - Location-selecting text entry
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
#include "location-entry.h"

#include <string.h>

/**
 * SECTION:location-entry
 * @Title: CafeWeatherLocationEntry
 *
 * A subclass of #CtkEntry that provides autocompletion on
 * #CafeWeatherLocation<!-- -->s
 */

G_DEFINE_TYPE (CafeWeatherLocationEntry, cafeweather_location_entry, CTK_TYPE_ENTRY)

enum {
    PROP_0,

    PROP_TOP,
    PROP_LOCATION,

    LAST_PROP
};

static void cafeweather_location_entry_build_model (CafeWeatherLocationEntry *entry,
						 CafeWeatherLocation *top);
static void set_property (GObject *object, guint prop_id,
			  const GValue *value, GParamSpec *pspec);
static void get_property (GObject *object, guint prop_id,
			  GValue *value, GParamSpec *pspec);

enum
{
    CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME = 0,
    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION,
    CAFEWEATHER_LOCATION_ENTRY_COL_COMPARE_NAME,
    CAFEWEATHER_LOCATION_ENTRY_COL_SORT_NAME,
    CAFEWEATHER_LOCATION_ENTRY_NUM_COLUMNS
};

static gboolean matcher (CtkEntryCompletion *completion, const char *key,
			 CtkTreeIter *iter, gpointer user_data);
static gboolean match_selected (CtkEntryCompletion *completion,
				CtkTreeModel       *model,
				CtkTreeIter        *iter,
				gpointer            entry);
static void     entry_changed (CafeWeatherLocationEntry *entry);

static void
cafeweather_location_entry_init (CafeWeatherLocationEntry *entry)
{
    CtkEntryCompletion *completion;

    completion = ctk_entry_completion_new ();

    ctk_entry_completion_set_popup_set_width (completion, FALSE);
    ctk_entry_completion_set_text_column (completion, CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME);
    ctk_entry_completion_set_match_func (completion, matcher, NULL, NULL);

    g_signal_connect (completion, "match_selected",
		      G_CALLBACK (match_selected), entry);

    ctk_entry_set_completion (CTK_ENTRY (entry), completion);
    g_object_unref (completion);

    entry->custom_text = FALSE;
    g_signal_connect (entry, "changed",
		      G_CALLBACK (entry_changed), NULL);
}

static void
finalize (GObject *object)
{
    CafeWeatherLocationEntry *entry = CAFEWEATHER_LOCATION_ENTRY (object);

    if (entry->location)
	cafeweather_location_unref (entry->location);
    if (entry->top)
	cafeweather_location_unref (entry->top);

    G_OBJECT_CLASS (cafeweather_location_entry_parent_class)->finalize (object);
}

static void
cafeweather_location_entry_class_init (CafeWeatherLocationEntryClass *location_entry_class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (location_entry_class);

    object_class->finalize = finalize;
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    /* properties */
    g_object_class_install_property (
	object_class, PROP_TOP,
	g_param_spec_pointer ("top",
			      "Top Location",
			      "The CafeWeatherLocation whose children will be used to fill in the entry",
			      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (
	object_class, PROP_LOCATION,
	g_param_spec_pointer ("location",
			      "Location",
			      "The selected CafeWeatherLocation",
			      G_PARAM_READWRITE));
}

static void
set_property (GObject *object, guint prop_id,
	      const GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_TOP:
	cafeweather_location_entry_build_model (CAFEWEATHER_LOCATION_ENTRY (object),
					     g_value_get_pointer (value));
	break;
    case PROP_LOCATION:
	cafeweather_location_entry_set_location (CAFEWEATHER_LOCATION_ENTRY (object),
					      g_value_get_pointer (value));
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
    CafeWeatherLocationEntry *entry = CAFEWEATHER_LOCATION_ENTRY (object);

    switch (prop_id) {
    case PROP_LOCATION:
	g_value_set_pointer (value, entry->location);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void
entry_changed (CafeWeatherLocationEntry *entry)
{
    entry->custom_text = TRUE;
}

static void
set_location_internal (CafeWeatherLocationEntry *entry,
		       CtkTreeModel          *model,
		       CtkTreeIter           *iter)
{
    CafeWeatherLocation *loc;
    char *name;

    if (entry->location)
	cafeweather_location_unref (entry->location);

    if (iter) {
	ctk_tree_model_get (model, iter,
			    CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME, &name,
			    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, &loc,
			    -1);
	entry->location = cafeweather_location_ref (loc);
	ctk_entry_set_text (CTK_ENTRY (entry), name);
	entry->custom_text = FALSE;
	g_free (name);
    } else {
	entry->location = NULL;
	ctk_entry_set_text (CTK_ENTRY (entry), "");
	entry->custom_text = TRUE;
    }

    ctk_editable_select_region (CTK_EDITABLE (entry), 0, -1);
    g_object_notify (G_OBJECT (entry), "location");
}

/**
 * cafeweather_location_entry_set_location:
 * @entry: a #CafeWeatherLocationEntry
 * @loc: (allow-none): a #CafeWeatherLocation in @entry, or %NULL to
 * clear @entry
 *
 * Sets @entry's location to @loc, and updates the text of the
 * entry accordingly.
 **/
void
cafeweather_location_entry_set_location (CafeWeatherLocationEntry *entry,
				      CafeWeatherLocation      *loc)
{
    CtkEntryCompletion *completion;
    CtkTreeModel *model;
    CtkTreeIter iter;
    CafeWeatherLocation *cmploc;

    g_return_if_fail (CAFEWEATHER_IS_LOCATION_ENTRY (entry));

    completion = ctk_entry_get_completion (CTK_ENTRY (entry));
    model = ctk_entry_completion_get_model (completion);

    ctk_tree_model_get_iter_first (model, &iter);
    do {
	ctk_tree_model_get (model, &iter,
			    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, &cmploc,
			    -1);
	if (loc == cmploc) {
	    set_location_internal (entry, model, &iter);
	    return;
	}
    } while (ctk_tree_model_iter_next (model, &iter));

    set_location_internal (entry, model, NULL);
}

/**
 * cafeweather_location_entry_get_location:
 * @entry: a #CafeWeatherLocationEntry
 *
 * Gets the location that was set by a previous call to
 * cafeweather_location_entry_set_location() or was selected by the user.
 *
 * Return value: (transfer full) (allow-none): the selected location
 * (which you must unref when you are done with it), or %NULL if no
 * location is selected.
 **/
CafeWeatherLocation *
cafeweather_location_entry_get_location (CafeWeatherLocationEntry *entry)
{
    g_return_val_if_fail (CAFEWEATHER_IS_LOCATION_ENTRY (entry), NULL);

    if (entry->location)
	return cafeweather_location_ref (entry->location);
    else
	return NULL;
}

/**
 * cafeweather_location_entry_has_custom_text:
 * @entry: a #CafeWeatherLocationEntry
 *
 * Checks whether or not @entry's text has been modified by the user.
 * Note that this does not mean that no location is associated with @entry.
 * cafeweather_location_entry_get_location() should be used for this.
 *
 * Return value: %TRUE if @entry's text was modified by the user, or %FALSE if
 * it's set to the default text of a location.
 **/
gboolean
cafeweather_location_entry_has_custom_text (CafeWeatherLocationEntry *entry)
{
    g_return_val_if_fail (CAFEWEATHER_IS_LOCATION_ENTRY (entry), FALSE);

    return entry->custom_text;
}

/**
 * cafeweather_location_entry_set_city:
 * @entry: a #CafeWeatherLocationEntry
 * @city_name: (allow-none): the city name, or %NULL
 * @code: the METAR station code
 *
 * Sets @entry's location to a city with the given @code, and given
 * @city_name, if non-%NULL. If there is no matching city, sets
 * @entry's location to %NULL.
 *
 * Return value: %TRUE if @entry's location could be set to a matching city,
 * %FALSE otherwise.
 **/
gboolean
cafeweather_location_entry_set_city (CafeWeatherLocationEntry *entry,
				  const char            *city_name,
				  const char            *code)
{
    CtkEntryCompletion *completion;
    CtkTreeModel *model;
    CtkTreeIter iter;
    CafeWeatherLocation *cmploc;
    const char *cmpcode;
    char *cmpname;

    g_return_val_if_fail (CAFEWEATHER_IS_LOCATION_ENTRY (entry), FALSE);
    g_return_val_if_fail (code != NULL, FALSE);

    completion = ctk_entry_get_completion (CTK_ENTRY (entry));
    model = ctk_entry_completion_get_model (completion);

    ctk_tree_model_get_iter_first (model, &iter);
    do {
	ctk_tree_model_get (model, &iter,
			    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, &cmploc,
			    -1);

	cmpcode = cafeweather_location_get_code (cmploc);
	if (!cmpcode || strcmp (cmpcode, code) != 0)
	    continue;

	if (city_name) {
	    cmpname = cafeweather_location_get_city_name (cmploc);
	    if (!cmpname || strcmp (cmpname, city_name) != 0) {
		g_free (cmpname);
		continue;
	    }
	    g_free (cmpname);
	}

	set_location_internal (entry, model, &iter);
	return TRUE;
    } while (ctk_tree_model_iter_next (model, &iter));

    set_location_internal (entry, model, NULL);

    return FALSE;
}

static void
fill_location_entry_model (CtkTreeStore *store, CafeWeatherLocation *loc,
			   const char *parent_display_name,
			   const char *parent_compare_name)
{
    CafeWeatherLocation **children;
    char *display_name, *compare_name;
    CtkTreeIter iter;
    int i;

    children = cafeweather_location_get_children (loc);

    switch (cafeweather_location_get_level (loc)) {
    case CAFEWEATHER_LOCATION_WORLD:
    case CAFEWEATHER_LOCATION_REGION:
    case CAFEWEATHER_LOCATION_ADM2:
	/* Ignore these levels of hierarchy; just recurse, passing on
	 * the names from the parent node.
	 */
	for (i = 0; children[i]; i++) {
	    fill_location_entry_model (store, children[i],
				       parent_display_name,
				       parent_compare_name);
	}
	break;

    case CAFEWEATHER_LOCATION_COUNTRY:
	/* Recurse, initializing the names to the country name */
	for (i = 0; children[i]; i++) {
	    fill_location_entry_model (store, children[i],
				       cafeweather_location_get_name (loc),
				       cafeweather_location_get_sort_name (loc));
	}
	break;

    case CAFEWEATHER_LOCATION_ADM1:
	/* Recurse, adding the ADM1 name to the country name */
	display_name = g_strdup_printf ("%s, %s", cafeweather_location_get_name (loc), parent_display_name);
	compare_name = g_strdup_printf ("%s, %s", cafeweather_location_get_sort_name (loc), parent_compare_name);

	for (i = 0; children[i]; i++) {
	    fill_location_entry_model (store, children[i],
				       display_name, compare_name);
	}

	g_free (display_name);
	g_free (compare_name);
	break;

    case CAFEWEATHER_LOCATION_CITY:
	if (children[0] && children[1]) {
	    /* If there are multiple (<location>) children, add a line
	     * for each of them.
	     */
	    for (i = 0; children[i]; i++) {
		display_name = g_strdup_printf ("%s (%s), %s",
						cafeweather_location_get_name (loc),
						cafeweather_location_get_name (children[i]),
						parent_display_name);
		compare_name = g_strdup_printf ("%s (%s), %s",
						cafeweather_location_get_sort_name (loc),
						cafeweather_location_get_sort_name (children[i]),
						parent_compare_name);

		ctk_tree_store_append (store, &iter, NULL);
		ctk_tree_store_set (store, &iter,
				    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, children[i],
				    CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME, display_name,
				    CAFEWEATHER_LOCATION_ENTRY_COL_COMPARE_NAME, compare_name,
				    -1);

		g_free (display_name);
		g_free (compare_name);
	    }
	} else if (children[0]) {
	    /* Else there's only one location. This is a mix of the
	     * city-with-multiple-location case above and the
	     * location-with-no-city case below.
	     */
	    display_name = g_strdup_printf ("%s, %s",
					    cafeweather_location_get_name (loc),
					    parent_display_name);
	    compare_name = g_strdup_printf ("%s, %s",
					    cafeweather_location_get_sort_name (loc),
					    parent_compare_name);

	    ctk_tree_store_append (store, &iter, NULL);
	    ctk_tree_store_set (store, &iter,
				CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, children[0],
				CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME, display_name,
				CAFEWEATHER_LOCATION_ENTRY_COL_COMPARE_NAME, compare_name,
				-1);

	    g_free (display_name);
	    g_free (compare_name);
	}
	break;

    case CAFEWEATHER_LOCATION_WEATHER_STATION:
	/* <location> with no parent <city>, or <city> with a single
	 * child <location>.
	 */
	display_name = g_strdup_printf ("%s, %s",
					cafeweather_location_get_name (loc),
					parent_display_name);
	compare_name = g_strdup_printf ("%s, %s",
					cafeweather_location_get_sort_name (loc),
					parent_compare_name);

	ctk_tree_store_append (store, &iter, NULL);
	ctk_tree_store_set (store, &iter,
			    CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, loc,
			    CAFEWEATHER_LOCATION_ENTRY_COL_DISPLAY_NAME, display_name,
			    CAFEWEATHER_LOCATION_ENTRY_COL_COMPARE_NAME, compare_name,
			    -1);

	g_free (display_name);
	g_free (compare_name);
	break;
    }

    cafeweather_location_free_children (loc, children);
}

static void
cafeweather_location_entry_build_model (CafeWeatherLocationEntry *entry,
				     CafeWeatherLocation *top)
{
    CtkTreeStore *store = NULL;

    g_return_if_fail (CAFEWEATHER_IS_LOCATION_ENTRY (entry));
    entry->top = cafeweather_location_ref (top);

    store = ctk_tree_store_new (4, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING);
    fill_location_entry_model (store, top, NULL, NULL);
    ctk_entry_completion_set_model (ctk_entry_get_completion (CTK_ENTRY (entry)),
				    CTK_TREE_MODEL (store));
    g_object_unref (store);
}

static char *
find_word (const char *full_name, const char *word, int word_len,
	   gboolean whole_word, gboolean is_first_word)
{
    char *p = (char *)full_name - 1;

    while ((p = strchr (p + 1, *word))) {
	if (strncmp (p, word, word_len) != 0)
	    continue;

	if (p > (char *)full_name) {
	    char *prev = g_utf8_prev_char (p);

	    /* Make sure p points to the start of a word */
	    if (g_unichar_isalpha (g_utf8_get_char (prev)))
		continue;

	    /* If we're matching the first word of the key, it has to
	     * match the first word of the location, city, state, or
	     * country. Eg, it either matches the start of the string
	     * (which we already know it doesn't at this point) or
	     * it is preceded by the string ", " (which isn't actually
	     * a perfect test. FIXME)
	     */
	    if (is_first_word) {
		if (prev == (char *)full_name || strncmp (prev - 1, ", ", 2) != 0)
		    continue;
	    }
	}

	if (whole_word && g_unichar_isalpha (g_utf8_get_char (p + word_len)))
	    continue;

	return p;
    }
    return NULL;
}

static gboolean
matcher (CtkEntryCompletion *completion,
	 const char         *key,
	 CtkTreeIter        *iter,
	 gpointer            user_data G_GNUC_UNUSED)
{
    char *name, *name_mem;
    CafeWeatherLocation *loc;
    gboolean is_first_word = TRUE, match;
    int len;

    ctk_tree_model_get (ctk_entry_completion_get_model (completion), iter,
			CAFEWEATHER_LOCATION_ENTRY_COL_COMPARE_NAME, &name_mem,
			CAFEWEATHER_LOCATION_ENTRY_COL_LOCATION, &loc,
			-1);
    name = name_mem;

    if (!loc) {
	g_free (name_mem);
	return FALSE;
    }

    /* All but the last word in KEY must match a full word from NAME,
     * in order (but possibly skipping some words from NAME).
     */
    len = strcspn (key, " ");
    while (key[len]) {
	name = find_word (name, key, len, TRUE, is_first_word);
	if (!name) {
	    g_free (name_mem);
	    return FALSE;
	}

	key += len;
	while (*key && !g_unichar_isalpha (g_utf8_get_char (key)))
	    key = g_utf8_next_char (key);
	while (*name && !g_unichar_isalpha (g_utf8_get_char (name)))
	    name = g_utf8_next_char (name);

	len = strcspn (key, " ");
	is_first_word = FALSE;
    }

    /* The last word in KEY must match a prefix of a following word in NAME */
    match = find_word (name, key, strlen (key), FALSE, is_first_word) != NULL;
    g_free (name_mem);
    return match;
}

static gboolean
match_selected (CtkEntryCompletion *completion G_GNUC_UNUSED,
		CtkTreeModel       *model,
		CtkTreeIter        *iter,
		gpointer            entry)
{
    set_location_internal (entry, model, iter);
    return TRUE;
}

/**
 * cafeweather_location_entry_new:
 * @top: the top-level location for the entry.
 *
 * Creates a new #CafeWeatherLocationEntry.
 *
 * @top will normally be a location returned from
 * cafeweather_location_new_world(), but you can create an entry that
 * only accepts a smaller set of locations if you want.
 *
 * Return value: the new #CafeWeatherLocationEntry
 **/
CtkWidget *
cafeweather_location_entry_new (CafeWeatherLocation *top)
{
    return g_object_new (CAFEWEATHER_TYPE_LOCATION_ENTRY,
			 "top", top,
			 NULL);
}
