// vim: ts=4 sw=4 colorcolumn=80

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Type declaration */

#define DEMO_TYPE_ANIMAL demo_animal_get_type()
G_DECLARE_DERIVABLE_TYPE (DemoAnimal, demo_animal, DEMO, ANIMAL, GtkWidget)

/* Class declaration */

struct _DemoAnimalClass
{
	GtkWidgetClass parent_class;

	void (* come)		(DemoAnimal *self);
	void (* make_sound)	(DemoAnimal *self,
			guint count);

	/* It is idiomatic in GObject to add some "padding" to allow for more
	 * virtual functions to be added in the future without breaking ABI.
	 */
	gpointer padding[12];
};

/* Method declarations */

GtkWidget *		demo_animal_new (void);
void			demo_animal_make_sound	(DemoAnimal *self, guint count);

G_END_DECLS
