// vim: colorcolumn=80 ts=4 sw=4

#include "demo-cat.h"

static void
print_cat_name (const char *name)
{
	if (! name)
		g_print ("Your cat has no name.\n");
	else
		g_print ("Your cat's name is: %s\n", name);
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

	{
		char *name;

		/* Test printing our cat's name before and after setting the "name"
		 * property. Note that "name" is a property of DemoAnimal, and not
		 * specifically of DemoCat.
		 */
		g_object_get (cat, "name", &name, NULL);
		print_cat_name (name);
		g_object_set (cat, "name", "Jeepers", NULL);
		g_object_get (cat, "name", &name, NULL);
		print_cat_name (name);
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
