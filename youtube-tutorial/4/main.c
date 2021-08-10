/* vim: colorcolumn=80 ts=4 sw=4
 */
/* main.c:  Boilerplate stub to view our demo widget.
 */

#include "demowidget.h"

static void
foobar_cb (DemoWidget *demo, gpointer user_data)
{
	g_print ("DemoWidget says hello.\n");
}

static void
activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *demo;
	GtkWidget *button;

	(void)user_data;	/* unused */

	window = gtk_application_window_new (app);

	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_set_homogeneous (GTK_BOX(box), TRUE);

	button = gtk_button_new_with_label ("Yo");

	demo = demo_widget_new ();
	gtk_widget_set_hexpand (demo, TRUE);

	gtk_box_append (GTK_BOX(box), demo);
	gtk_box_append (GTK_BOX(box), button);
	
	/* Set label of DemoWidget using the standard GObject setter: */

	g_object_set (demo,
			"label", "foo bar baz",
			NULL);
	
	/* Connect to our custom signal */
	g_signal_connect (demo, "foobar", G_CALLBACK(foobar_cb), NULL);

	/* Connect button signal to toggle widget label selection on and off */
	g_signal_connect_swapped (button, "clicked",
			G_CALLBACK(demo_widget_toggle_selection), demo);

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
