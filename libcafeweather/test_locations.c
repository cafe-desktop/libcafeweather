
#define CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include <ctk/ctk.h>
#include "location-entry.h"
#include "timezone-menu.h"

static void
deleted (CtkWidget *widget G_GNUC_UNUSED,
	 CdkEvent  *event G_GNUC_UNUSED,
	 gpointer   data G_GNUC_UNUSED)
{
    ctk_main_quit ();
}

static void
location_changed (GObject    *object,
		  GParamSpec *param G_GNUC_UNUSED,
		  gpointer    tzmenu)
{
    CafeWeatherLocationEntry *entry = CAFEWEATHER_LOCATION_ENTRY (object);
    CafeWeatherLocation *loc;
    CafeWeatherTimezone *zone;

    loc = cafeweather_location_entry_get_location (entry);
    g_return_if_fail (loc != NULL);
    zone = cafeweather_location_get_timezone (loc);
    if (zone)
	cafeweather_timezone_menu_set_tzid (tzmenu, cafeweather_timezone_get_tzid (zone));
    else
	cafeweather_timezone_menu_set_tzid (tzmenu, NULL);
    if (zone)
	cafeweather_timezone_unref (zone);
    cafeweather_location_unref (loc);
}

int
main (int argc, char **argv)
{
    CafeWeatherLocation *loc;
    CtkWidget *window, *vbox, *entry;
    CtkWidget *combo;
    ctk_init (&argc, &argv);

    window = ctk_window_new (CTK_WINDOW_TOPLEVEL);
    ctk_window_set_title (CTK_WINDOW (window), "location");
    ctk_container_set_border_width (CTK_CONTAINER (window), 8);
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (deleted), NULL);

    vbox = ctk_box_new (CTK_ORIENTATION_VERTICAL, 8);
    ctk_container_add (CTK_CONTAINER (window), vbox);

    loc = cafeweather_location_new_world (FALSE);
    entry = cafeweather_location_entry_new (loc);
    ctk_widget_set_size_request (entry, 400, -1);
    ctk_box_pack_start (CTK_BOX (vbox), entry, FALSE, TRUE, 0);

    combo = cafeweather_timezone_menu_new (loc);
    cafeweather_location_unref (loc);
    ctk_box_pack_start (CTK_BOX (vbox), combo, FALSE, TRUE, 0);

    g_signal_connect (entry, "notify::location",
		      G_CALLBACK (location_changed), combo);

    ctk_widget_show_all (window);

    ctk_main ();

    return 0;
}
