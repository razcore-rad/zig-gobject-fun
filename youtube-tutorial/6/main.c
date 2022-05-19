// vim: colorcolumn=80 ts=4 sw=4

#include "demo-cat.h"

/* nb: There is also a way to do this without a using a callback if
 * you know some special trickery with GObject... Note that the prototype
 * for a 'pet' callback takes 2 parameters, and the first will be a DemoCat
 * which will also have to be a DemoAnimal. What if we could then marshall
 * the user_data pointer into an integer which demo_animal_make_sound could
 * accept?
 */
static void
pet_cat_cb (DemoCat *cat, int enjoyment_factor, gpointer user_data)
{
	demo_animal_make_sound (DEMO_ANIMAL(cat), enjoyment_factor);
}

/* Can modify prototype like so, to clean up code a bit. */
static void
fed_cat_cb (DemoCat *cat, GParamSpec *pspec, gpointer user_data)
{
	if (demo_cat_get_fed (cat))
		demo_cat_purr (cat);
}

static void
activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *cat;
	GtkWidget *feed_button;

	/* Setup our widgets for our window. Note that this can get unwieldy!
	 * As an exercise, try doing something similar to this using a .ui file.
	 */
	window = gtk_application_window_new (app);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	cat = demo_cat_new ();
	feed_button = gtk_button_new_with_mnemonic ("_Feed the Kitty!!");

	gtk_widget_set_hexpand (cat, TRUE);
	gtk_widget_set_vexpand (cat, TRUE);
	gtk_widget_set_hexpand (feed_button, FALSE);
	gtk_widget_set_halign (feed_button, GTK_ALIGN_START);

	gtk_box_append (GTK_BOX(box), feed_button);
	gtk_box_append (GTK_BOX(box), cat);

	/* Setup signals */
	g_signal_connect (cat, "pet", G_CALLBACK(pet_cat_cb), NULL);
	g_signal_connect_swapped (feed_button, "clicked", G_CALLBACK(demo_cat_feed), cat);
	g_signal_connect (cat, "notify::fed", G_CALLBACK(fed_cat_cb), NULL);

	/* Setup Window */
	gtk_window_set_title (GTK_WINDOW(window), "Window");
	gtk_window_set_default_size (GTK_WINDOW(window), 700, 600);
	gtk_window_set_child (GTK_WINDOW(window), box);

	gtk_widget_show (window);
}

int
main (int argc, char *argv[])
{
	GtkApplication *app;
	int status;

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref (app);

	return status;
}
