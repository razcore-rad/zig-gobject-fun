// vim: colorcolumn=80 ts=4 sw=4

#include "demowidget.h"

/* GLOBALS FOR PROPERTIES */

typedef enum
{
	PROP_LABEL = 1,
	N_PROPERTIES
} DemoWidgetProperty;

static GParamSpec *properties[N_PROPERTIES];

/* INTERNAL DECLARATIONS */

/* nb: these are not generated by the macros automatically. */

static void demo_widget_dispose (GObject *object);
static void demo_widget_finalize (GObject *object);

/* GOBJECT DEFINITION */

struct _DemoWidget
{
	GtkWidget parent_instance;

	GtkWidget *button;
};

G_DEFINE_TYPE (DemoWidget, demo_widget, GTK_TYPE_WIDGET)

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
		case PROP_LABEL:
			demo_widget_set_label (self, g_value_get_string (value));
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
		case PROP_LABEL:

			g_value_set_string (value,
					gtk_button_get_label (GTK_BUTTON(self->button)));
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

	self->button = gtk_button_new_with_label ("Hello, world!");
	gtk_widget_set_hexpand (self->button, TRUE);
	gtk_widget_set_parent (self->button, widget);
}

static void
demo_widget_dispose (GObject *object)
{
	DemoWidget *self = DEMO_WIDGET(object);

	g_clear_pointer (&self->button, gtk_widget_unparent);

	/* Final step: Chain up */

	G_OBJECT_CLASS(demo_widget_parent_class)->dispose(object);
}

static void
demo_widget_finalize (GObject *gobject)
{
	/* here, you would free stuff. I've got nuthin' for ya. */

	/* --- */

	/* Always chain up to the parent class; as with dispose(), finalize() is
	 * autogenerated and thus always guaranteed to exist on the parent's class
	 * virtual function table
	 */
	
	G_OBJECT_CLASS(demo_widget_parent_class)->finalize(gobject);
}

static void
demo_widget_class_init (DemoWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GParamFlags prop_flags = G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		G_PARAM_EXPLICIT_NOTIFY;

	/* map vfuncs */

	object_class->dispose = demo_widget_dispose;
	object_class->finalize = demo_widget_finalize;
	object_class->set_property = demo_widget_set_property;
	object_class->get_property = demo_widget_get_property;

	/* properties */

	properties[PROP_LABEL] = g_param_spec_string ("label",
			"Button label",
			"Textual label for our lovely button",
			/* default: */	"Hello, world!",
			prop_flags);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);

	/* layout manager */

	gtk_widget_class_set_layout_manager_type (GTK_WIDGET_CLASS(klass),
			GTK_TYPE_BOX_LAYOUT);
}

void
demo_widget_set_label (DemoWidget *self, const char *label)
{
	gtk_button_set_label (GTK_BUTTON(self->button), label);
	g_object_notify_by_pspec (G_OBJECT(self), properties[PROP_LABEL]);
}

GtkWidget *
demo_widget_new (void)
{
	return g_object_new (DEMO_TYPE_WIDGET, NULL);
}
