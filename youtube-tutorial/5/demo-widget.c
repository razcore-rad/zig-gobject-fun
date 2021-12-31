// vim: colorcolumn=85 ts=4 sw=4

#include "demo-widget.h"

/* PROPERTIES */

enum
{
	PROP_ONE = 1,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

/* GLOBALS FOR SIGNALS */

enum signal_types {
	SIGNAL_ONE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

/* GOBJECT DEFINITION */

/* G_DECLARE_DERIVABLE_TYPE already did something like this:
struct _DemoWidget
{
	GtkWidget parent_instance;
};
*/

typedef struct
{
	GtkWidget *frame;
	GtkWidget *frame_box;
} DemoWidgetPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (DemoWidget, demo_widget, GTK_TYPE_WIDGET)

/* PROPERTIES - GETTERS AND SETTERS */

static void
demo_widget_set_property (GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	DemoWidget *self = DEMO_WIDGET(object);

	switch (property_id)
	{
		case PROP_ONE:
			/* --- */
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
demo_widget_get_property (GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	DemoWidget *self = DEMO_WIDGET(object);

	switch (property_id)
	{
		case PROP_ONE:
			/* --- */
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* METHOD DEFINITIONS */

static void
demo_widget_init (DemoWidget *self)
{
	GtkWidget *widget = GTK_WIDGET(self);

	gtk_widget_init_template (widget);
}

static void
demo_widget_dispose (GObject *object)
{
	DemoWidget *self = DEMO_WIDGET(object);
								/* generated from macro */
	DemoWidgetPrivate *priv = demo_widget_get_instance_private (self);

	g_clear_pointer (&priv->frame, gtk_widget_unparent);

	/* Chain up */
	G_OBJECT_CLASS(demo_widget_parent_class)->dispose (object);
}

static void
demo_widget_finalize (GObject *object)
{
	DemoWidget *self = DEMO_WIDGET(object);

	/* Chain up */
	G_OBJECT_CLASS(demo_widget_parent_class)->finalize (object);
}

static void
demo_widget_class_init (DemoWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	object_class->dispose =  demo_widget_dispose;
	object_class->finalize = demo_widget_finalize;

	object_class->set_property = demo_widget_set_property;
	object_class->get_property = demo_widget_get_property;

	/* TEMPLATE */

	gtk_widget_class_set_template_from_resource (widget_class, "/demo-widget.ui");
	gtk_widget_class_bind_template_child_private (widget_class, DemoWidget, frame);
	gtk_widget_class_bind_template_child_private (widget_class, DemoWidget, frame_box);

	/* LAYOUT MANAGER */

	gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);

	/* PROPERTIES */

	properties[PROP_ONE] = g_param_spec_string ("property-one",
			"Property one",
			"Our lovely first property",
			/* default: */	"Hello, world!",
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);

	/* SIGNALS */

	signals[SIGNAL_ONE] = g_signal_new_class_handler ("signal-one",
			G_OBJECT_CLASS_TYPE (object_class),
			G_SIGNAL_RUN_LAST,
		/* no default C function */
			NULL,
		/* defaults for accumulator, marshaller &c. */
			NULL, NULL, NULL,	
		/* No return type or params. */
			G_TYPE_NONE, 0);

}

/* PUBLIC METHOD DEFINITIONS */

GtkWidget *
demo_widget_new (void)
{
	return g_object_new (DEMO_TYPE_WIDGET, NULL);
}
