// vim: ts=4 sw=4 colorcolumn=80

#include "demo-cat.h"

/* PROPERTIES */

enum
{
	FED = 1,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

/* GLOBALS FOR SIGNALS */

enum signal_types {
	PET,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

/* GOBJECT DEFINITION */

struct _DemoCat
{
	DemoAnimal parent_instance;

	GtkWidget *grid;
	GtkWidget *cat_image;
	GtkWidget *speech_bubble_revealer;
	GtkWidget *speech_bubble;
	GtkWidget *speech_bubble_label;
	guint speech_bubble_timer;
	gboolean fed;
};

G_DEFINE_TYPE (DemoCat, demo_cat, DEMO_TYPE_ANIMAL)

/* PROPERTIES - GETTERS AND SETTERS */

static void
demo_cat_set_property (GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	DemoCat *self = DEMO_CAT(object);

	switch (property_id)
	{
		case FED:
			self->fed = g_value_get_boolean (value);
			g_object_notify_by_pspec (object, properties[FED]);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
demo_cat_get_property (GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	DemoCat *self = DEMO_CAT(object);

	switch (property_id)
	{
		case FED:
			g_value_set_boolean (value, self->fed);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* PRIVATE FUNCTIONS */

static void
emit_pet (DemoCat *self, double offset_x, double offset_y)
{
	int enjoyment_factor;

	/* Calculate an 'enjoyment factor' based on how long the pet of the cat
	 * was. Since a really small pet can lead to a zero, clamp it at 1.
	 */
	enjoyment_factor = MAX ((ABS(offset_x) + ABS(offset_y)) / 20, 1);

	g_signal_emit (self, signals[PET], 0, enjoyment_factor);
}

/* GSourceFunc */
static gboolean
speech_bubble_timeout (gpointer user_data)
{
	DemoCat *self = DEMO_CAT (user_data);

	gtk_revealer_set_reveal_child (GTK_REVEALER(self->speech_bubble_revealer), FALSE);

	self->speech_bubble_timer = 0;
	return G_SOURCE_REMOVE;
}

/* VIRTUAL FUNCTION IMPLEMENTATIONS */

static void
demo_cat_real_make_sound (DemoAnimal *animal, guint count)
{
	DemoCat *self = DEMO_CAT(animal);
	g_autoptr(GString) meow_string = g_string_new (NULL);

	if (! count)
		return;

	if (self->speech_bubble_timer)
		g_source_remove (self->speech_bubble_timer);
	self->speech_bubble_timer = g_timeout_add_seconds (5, speech_bubble_timeout, self);

	gtk_revealer_set_reveal_child (GTK_REVEALER(self->speech_bubble_revealer),
			TRUE);

	for (guint i = 0; i < count; ++i)
	{
		 g_string_append (meow_string, "Meow ");
	}

	gtk_label_set_text (GTK_LABEL(self->speech_bubble_label), meow_string->str);
}

/* METHOD DEFINITIONS */

static void
demo_cat_init (DemoCat *self)
{
	GtkBuilder *builder = gtk_builder_new_from_file ("demo-cat.ui");
	GtkCssProvider *provider;
	GtkStyleContext *context;
	GtkGesture *drag;

	self->grid = GTK_WIDGET(gtk_builder_get_object (builder, "grid"));
	self->cat_image = GTK_WIDGET(gtk_builder_get_object (builder, "cat_image"));
	self->speech_bubble_revealer = GTK_WIDGET(gtk_builder_get_object (builder, "speech_bubble_revealer"));
	self->speech_bubble = GTK_WIDGET(gtk_builder_get_object (builder, "speech_bubble"));
	self->speech_bubble_label = GTK_WIDGET(gtk_builder_get_object (builder, "speech_bubble_label"));

	/* Sinks all references */
	gtk_widget_set_parent (self->grid, GTK_WIDGET(self));

	/* Make sure you wait for references to be 'sunk' before unreffing
	 * GtkBuilder.
	 */
	g_object_unref (builder);

	context = gtk_widget_get_style_context (GTK_WIDGET(self->speech_bubble));
	provider = gtk_css_provider_new ();

	gtk_css_provider_load_from_path (provider, "demo-cat.css");
	gtk_style_context_add_provider (context,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	drag = gtk_gesture_drag_new ();
	gtk_widget_add_controller (self->cat_image, GTK_EVENT_CONTROLLER(drag));
	g_signal_connect_swapped (drag, "drag-end", G_CALLBACK(emit_pet), self);
}

static void
demo_cat_dispose (GObject *object)
{
	DemoCat *self = DEMO_CAT(object);

	/* Chain up */
	G_OBJECT_CLASS(demo_cat_parent_class)->dispose (object);
}

static void
demo_cat_finalize (GObject *object)
{
	DemoCat *self = DEMO_CAT(object);

	/* Chain up */
	G_OBJECT_CLASS(demo_cat_parent_class)->finalize (object);
}

static void
demo_cat_class_init (DemoCatClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	object_class->dispose =  demo_cat_dispose;
	object_class->finalize = demo_cat_finalize;

	object_class->set_property = demo_cat_set_property;
	object_class->get_property = demo_cat_get_property;

	DEMO_ANIMAL_CLASS(object_class)->make_sound = demo_cat_real_make_sound;

	/* SIGNALS */

	signals[PET] = g_signal_new_class_handler ("pet",
			G_OBJECT_CLASS_TYPE (object_class),
			G_SIGNAL_RUN_LAST,
		/* no default C function */
			NULL,
		/* defaults for accumulator, marshaller &c. */
			NULL, NULL, NULL,	
		/* No return type */
			G_TYPE_NONE,
		/* 1 param: enjoyment factor (integer) */
			1,
			G_TYPE_INT);

	/* PROPERTIES */

	properties[FED] = g_param_spec_boolean ("fed",
			NULL, NULL,			/* No nick or blurb: recent recommendation. */
			FALSE,				/* default */
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

/* PUBLIC METHOD DEFINITIONS */

GtkWidget *
demo_cat_new (void)
{
	return g_object_new (DEMO_TYPE_CAT, NULL);
}

void
demo_cat_purr (DemoCat *cat)
{
	g_print ("Prrrrrr!\n");
}

void
demo_cat_feed (DemoCat *cat)
{
	g_object_set (cat, "fed", TRUE, NULL);
}

/* A bit nicer API, and less overhead than a g_object_get call. */
gboolean
demo_cat_get_fed (DemoCat *cat)
{
	return cat->fed;
}
