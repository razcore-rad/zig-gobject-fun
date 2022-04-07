// vim: ts=4 sw=4 colorcolumn=80

#include "demo-cat.h"

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

struct _DemoCat
{
	DemoAnimal parent_instance;
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
demo_cat_get_property (GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	DemoCat *self = DEMO_CAT(object);

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
demo_cat_init (DemoCat *self)
{
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

	object_class->dispose =  demo_cat_dispose;
	object_class->finalize = demo_cat_finalize;

	object_class->set_property = demo_cat_set_property;
	object_class->get_property = demo_cat_get_property;

#if 0
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
#endif
}

/* PUBLIC METHOD DEFINITIONS */

GtkWidget *
demo_cat_new (void)
{
	return g_object_new (DEMO_TYPE_CAT, NULL);
}
