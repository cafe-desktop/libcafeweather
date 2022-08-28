
#define CAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include <gtk/gtk.h>
#include "location-entry.h"
#include "timezone-menu.h"

static void
deleted (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

static void
location_changed (GObject *object, GParamSpec *param, gpointer tzmenu)
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
    GtkWidget *window, *vbox, *entry;
    GtkWidget *combo;
    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "location");
    gtk_container_set_border_width (GTK_CONTAINER (window), 8);
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (deleted), NULL);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    loc = cafeweather_location_new_world (FALSE);
    entry = cafeweather_location_entry_new (loc);
    gtk_widget_set_size_request (entry, 400, -1);
    gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, TRUE, 0);

    combo = cafeweather_timezone_menu_new (loc);
    cafeweather_location_unref (loc);
    gtk_box_pack_start (GTK_BOX (vbox), combo, FALSE, TRUE, 0);

    g_signal_connect (entry, "notify::location",
		      G_CALLBACK (location_changed), combo);

    gtk_widget_show_all (window);

    gtk_main ();

    return 0;
}
