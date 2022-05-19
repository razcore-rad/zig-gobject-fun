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

static void
activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *cat;

	window = gtk_application_window_new (app);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	cat = demo_cat_new ();

	g_signal_connect (cat, "pet", G_CALLBACK(pet_cat_cb), NULL);


	gtk_widget_set_hexpand (cat, TRUE);
	gtk_box_append (GTK_BOX(box), cat);

	{
		gboolean cat_is_fed = FALSE;

		g_object_set (cat, "fed", TRUE, NULL);
		g_object_get (cat, "fed", &cat_is_fed, NULL);

		if (cat_is_fed)
			demo_cat_purr (DEMO_CAT(cat));
	}
	
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
