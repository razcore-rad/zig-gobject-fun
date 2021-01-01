#include "custom.h"

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *demo;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
	demo = demo_widget_new();

	gtk_box_pack_start(GTK_BOX(box), demo, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
