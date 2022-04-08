#!/bin/bash

NAMESPACE="demo"
OBJECT="widget"
PARENT_INSTANCE="GtkWidget"
PARENT_TYPE="GTK_TYPE_WIDGET"
NEW_METHOD_RETURN_TYPE="GtkWidget"
PROPERTIES=1
SIGNALS=1
DERIVABLE=1

#########

header_file="${NAMESPACE}-${OBJECT}.h"
source_file="${NAMESPACE}-${OBJECT}.c"

# GENERATE HEADER FILE

cat << EOF > $header_file
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Type declaration */

#define ${NAMESPACE^^}_TYPE_${OBJECT^^} ${NAMESPACE}_${OBJECT}_get_type()
EOF

if [ $DERIVABLE -eq 1 ]; then
	cat << EOF >> $header_file
G_DECLARE_DERIVABLE_TYPE (${NAMESPACE^}${OBJECT^}, ${NAMESPACE}_${OBJECT}, ${NAMESPACE^^}, ${OBJECT^^}, ${PARENT_INSTANCE})

/* Class declaration */

struct _${NAMESPACE^}${OBJECT^}Class
{
	${PARENT_INSTANCE}Class parent_class;

	void (* do_stuff)	(${NAMESPACE^}${OBJECT^} *self);

	gpointer padding[12];
};
EOF
else	# FINAL
	cat << EOF >> $header_file
G_DECLARE_FINAL_TYPE (${NAMESPACE^}${OBJECT^}, ${NAMESPACE}_${OBJECT}, ${NAMESPACE^^}, ${OBJECT^^}, ${PARENT_INSTANCE})
EOF
fi

cat << EOF >> $header_file

/* Method declarations */

${NEW_METHOD_RETURN_TYPE} *	${NAMESPACE}_${OBJECT}_new (void);

G_END_DECLS
EOF

# GENERATE SOURCE FILE

cat << EOF > $source_file
#include "${NAMESPACE}-${OBJECT}.h"

EOF

if [ $PROPERTIES -ne 0 ]; then
	cat << EOF >> $source_file
/* PROPERTIES */

enum
{
	PROP_ONE = 1,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

EOF
fi

if [ $SIGNALS -ne 0 ]; then
	cat << EOF >> $source_file
/* GLOBALS FOR SIGNALS */

enum signal_types {
	SIGNAL_ONE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

EOF
fi

if [ $DERIVABLE -eq 1 ]; then
	cat << EOF >> $source_file
/* PRIVATE DATA */

typedef struct
{
	/* Private data goes here. */
} ${NAMESPACE^}${OBJECT^}Private;

G_DEFINE_TYPE_WITH_PRIVATE (${NAMESPACE^}${OBJECT^}, ${NAMESPACE}_${OBJECT}, ${PARENT_TYPE})

EOF
else	# FINAL
	cat << EOF >> $source_file
/* GOBJECT DEFINITION */

struct _${NAMESPACE^}${OBJECT^}
{
	${PARENT_INSTANCE} parent_instance;
};

G_DEFINE_TYPE (${NAMESPACE^}${OBJECT^}, ${NAMESPACE}_${OBJECT}, ${PARENT_TYPE})

EOF
fi

if [ $PROPERTIES -ne 0 ]; then
	cat << EOF >> $source_file
/* PROPERTIES - GETTERS AND SETTERS */

static void
${NAMESPACE}_${OBJECT}_set_property (GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	${NAMESPACE^}${OBJECT^} *self = ${NAMESPACE^^}_${OBJECT^^}(object);

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
${NAMESPACE}_${OBJECT}_get_property (GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	${NAMESPACE^}${OBJECT^} *self = ${NAMESPACE^^}_${OBJECT^^}(object);

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

EOF
fi

cat << EOF >> $source_file
/* METHOD DEFINITIONS */

static void
${NAMESPACE}_${OBJECT}_init (${NAMESPACE^}${OBJECT^} *self)
{
}

static void
${NAMESPACE}_${OBJECT}_dispose (GObject *object)
{
	${NAMESPACE^}${OBJECT^} *self = ${NAMESPACE^^}_${OBJECT^^}(object);

	/* Chain up */
	G_OBJECT_CLASS(${NAMESPACE}_${OBJECT}_parent_class)->dispose (object);
}

static void
${NAMESPACE}_${OBJECT}_finalize (GObject *object)
{
	${NAMESPACE^}${OBJECT^} *self = ${NAMESPACE^^}_${OBJECT^^}(object);

	/* Chain up */
	G_OBJECT_CLASS(${NAMESPACE}_${OBJECT}_parent_class)->finalize (object);
}

static void
${NAMESPACE}_${OBJECT}_class_init (${NAMESPACE^}${OBJECT^}Class *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose =  ${NAMESPACE}_${OBJECT}_dispose;
	object_class->finalize = ${NAMESPACE}_${OBJECT}_finalize;

EOF
if [ $PROPERTIES -ne 0 ]; then
	cat << EOF >> $source_file
	object_class->set_property = ${NAMESPACE}_${OBJECT}_set_property;
	object_class->get_property = ${NAMESPACE}_${OBJECT}_get_property;

	/* PROPERTIES */

	properties[PROP_ONE] = g_param_spec_string ("property-one",
			"Property one",
			"Our lovely first property",
			/* default: */	"Hello, world!",
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);
EOF
fi

if [ $SIGNALS -ne 0 ]; then
	cat << EOF >> $source_file

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

EOF
fi

cat << EOF >> $source_file
}

/* PUBLIC METHOD DEFINITIONS */

${NEW_METHOD_RETURN_TYPE} *
${NAMESPACE}_${OBJECT}_new (void)
{
	return g_object_new (${NAMESPACE^^}_TYPE_${OBJECT^^}, NULL);
}
EOF
