#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Type declaration */

#define DEMO_TYPE_WIDGET demo_widget_get_type()
G_DECLARE_DERIVABLE_TYPE (DemoWidget, demo_widget, DEMO, WIDGET, GtkWidget)

struct _DemoWidgetClass
{
	GtkWidgetClass parent_class;

	void (*close) (DemoWidget *self);

	/* Padding to allow adding up to 12 new virtual functions without
	 * breaking ABI. */
	gpointer padding[12];
};

/* nb: will remove this in the next iteration, as we don't want this type of
 * bare object to be created directly by our library users */

GtkWidget *	demo_widget_new (void);

G_END_DECLS
