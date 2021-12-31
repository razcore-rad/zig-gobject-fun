/* vim: colorcolumn=80 ts=4 sw=4
 */
/* main.c:  Boilerplate stub to view our demo widget.
 */

#include "demo-widget.h"

static void
activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *demo;

	window = gtk_application_window_new (app);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	demo = demo_widget_new ();
	gtk_widget_set_hexpand (demo, TRUE);

	gtk_box_append (GTK_BOX(box), demo);
	
	gtk_window_set_title (GTK_WINDOW(window), "Window");
	gtk_window_set_default_size (GTK_WINDOW(window), 400, 200);
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
