// vim: colorcolumn=80 ts=4 sw=4

#include "demo-cat.h"

static void
activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *cat;

	window = gtk_application_window_new (app);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	cat = demo_cat_new ();

	{
		char *name;

		/* Test making our animal's sound -- NOT the recommended methodology. */
		DEMO_ANIMAL_GET_CLASS (cat)->make_sound (DEMO_ANIMAL(cat), 3);
	}

	gtk_widget_set_hexpand (cat, TRUE);
	gtk_box_append (GTK_BOX(box), cat);
	
	/* Setup Window */
	gtk_window_set_title (GTK_WINDOW(window), "Window");
	gtk_window_set_default_size (GTK_WINDOW(window), 400, 400);
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
