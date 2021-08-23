dnl	USAGE:
dnl
dnl	__G_GENERATE_FINAL_TYPE__(`DemoWidget', `demo_widget', `DEMO', `WIDGET', `GtkWidget', `GTK_TYPE_WIDGET', `demo-widget')
dnl
dnl	This will generate a .c and .h template for a final-type gobject. The first 5 arguments
dnl	are identical to G_DECLARE_FINAL_TYPE. Arguments 6 and 7 are the GType that will be plugged
dnl	into the G_DEFINE_TYPE macro in the .c file, and the filename that will be suffixed with {.c,.h},
dnl	respectively.
dnl
define(`__G_GENERATE_FINAL_TYPE__',
`syscmd(`cat << EOF > $7.h
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Type declaration */

#define $3_TYPE_$4 $2_get_type()
G_DECLARE_FINAL_TYPE ($1, $2, $3, $4, $5)

/* Method declarations */

$5 *	$2_new (void);

G_END_DECLS
EOF

cat << EOF > $7.c
#include "$7.h"

/* GOBJECT DEFINITION */

struct _$1
{
	$5 parent_instance;
};

G_DEFINE_TYPE ($1, $2, $6)


/* METHOD DEFINITIONS */

static void
$2_init ($1 *self)
{
}

static void
$2_dispose (GObject *object)
{
	$1 *self = $3_$4(object);

	/* Chain up */
	G_OBJECT_CLASS($2_parent_class)->dispose (object);
}

static void
$2_finalize (GObject *object)
{
	$1 *self = $3_$4(object);

	/* Chain up */
	G_OBJECT_CLASS($2_parent_class)->finalize (object);
}

static void
$2_class_init ($1Class *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = $2_dispose;
	object_class->finalize = $2_finalize;
}


/* PUBLIC METHOD DEFINITIONS */

$5 *
$2_new (void)
{
	return g_object_new ($3_TYPE_$4, NULL);
}
EOF')')dnl
