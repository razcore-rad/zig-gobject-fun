// vim: ts=4 sw=4 colorcolumn=80

#include "demo-animal.h"

/* PROPERTIES */

enum
{
	NAME = 1,
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

typedef struct
{
	char *name;

} DemoAnimalPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (DemoAnimal, demo_animal, GTK_TYPE_WIDGET)

/* PROPERTIES - GETTERS AND SETTERS */

static void
demo_animal_set_property (GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	DemoAnimal *self = DEMO_ANIMAL(object);
	DemoAnimalPrivate *priv = demo_animal_get_instance_private (self);

	switch (property_id)
	{
		case NAME:
			g_free (priv->name);
			priv->name = g_strdup (g_value_get_string (value));
			g_object_notify_by_pspec (object, properties[NAME]);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
demo_animal_get_property (GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	DemoAnimal *self = DEMO_ANIMAL(object);
	DemoAnimalPrivate *priv = demo_animal_get_instance_private (self);

	switch (property_id)
	{
		case NAME:
			g_value_set_string (value, priv->name);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* METHOD DEFINITIONS */

static void
demo_animal_init (DemoAnimal *self)
{
}

static void
demo_animal_dispose (GObject *object)
{
	DemoAnimal *self = DEMO_ANIMAL(object);

	/* Chain up */
	G_OBJECT_CLASS(demo_animal_parent_class)->dispose (object);
}

static void
demo_animal_finalize (GObject *object)
{
	DemoAnimal *self = DEMO_ANIMAL(object);

	/* Chain up */
	G_OBJECT_CLASS(demo_animal_parent_class)->finalize (object);
}

static void
demo_animal_class_init (DemoAnimalClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose =  demo_animal_dispose;
	object_class->finalize = demo_animal_finalize;

	object_class->set_property = demo_animal_set_property;
	object_class->get_property = demo_animal_get_property;

	/* PROPERTIES */

	properties[NAME] = g_param_spec_string ("name",
			"Name",
			"Name of the animal",
			/* default: */	NULL,
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
demo_animal_new (void)
{
	return g_object_new (DEMO_TYPE_ANIMAL, NULL);
}
