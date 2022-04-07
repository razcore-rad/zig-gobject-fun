// vim: ts=4 sw=4 colorcolumn=80

#pragma once

#include "demo-animal.h"

G_BEGIN_DECLS

/* Type declaration */

#define DEMO_TYPE_CAT demo_cat_get_type()
G_DECLARE_FINAL_TYPE (DemoCat, demo_cat, DEMO, CAT, DemoAnimal)

/* Method declarations */

GtkWidget *	demo_cat_new (void);

G_END_DECLS
