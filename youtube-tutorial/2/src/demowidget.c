// vim: colorcolumn=80 ts=4 sw=4

/* This is an extremely simple annotated .c file that implements a custom
 * widget in gtk4 with gobject. All it does is subclasses GtkWidget and creates
 * a widget which is just a button.
 *
 * Requires: glib >= 2.44, gtk4 >= 4.0.0
 *
 * This is a template for a FINAL (as opposed to DERIVABLE gobject type.
 * Final types are simpler. You do most of the work in the C file and just
 * expose a handful of public methods in the .h file.
 *
 * In other words, as it can't be subclassed further, you don't publicly
 * expose the guts of the object at all and it's just all defined and dealt
 * with using standard GObject/GTK functions.
 */

#include "demowidget.h"

/* GOBJECT DEFINITION */

/* First, create the main struct that has the guts of the object in it.
 * Follow the format of the underscore followed by the CamelCase type
 * name that you created in the header file. The typedef is autogenerated
 * by the macros and is expected to be defined here.
 */

struct _DemoWidget {
  /* subclass type - be sure you do *not* use a pointer. Keep this as the
   * first member.
   */
  GtkWidget parent_instance;

  /* Custom meat and potatoes go here. For now, we are just creating a
   * button. Here, you *do* use pointers when you want to incorporate other
   * objects in the structure.
   */
  GtkWidget *button;
};

/* once the main struct is defined as above, here is the next macro. Make sure
 * this goes in before doing anything else. Order matters.
 *
 * First two arguments to the macro are boilerplate. The last is the standard
 * capitalized TYPE macro of *what you're subclassing.* Lowest you can go is
 * G_TYPE_OBJECT for a raw gobject. In this case we're subclassing
 * GtkWidget, so that type macro is specified below.
 */
G_DEFINE_TYPE(DemoWidget, demo_widget, GTK_TYPE_WIDGET)

// Helper functions for Zig. Since we don't have access to the DemoWidget
// data type in Zig and are using an opaque pointer, we need a way to
// access the internal structure. We do this with getter and setters.
gpointer demo_widget_get_parent_class() { return demo_widget_parent_class; }

GtkWidget *demo_widget_get_button(DemoWidget *self) { return self->button; }
void demo_widget_set_button(DemoWidget *self, GtkWidget *button) {
  self->button = button;
}

/* METHOD DEFINITIONS */

// The following methods were moved to demowidget.zig:
//
// - demo_widget_init()
// - demo_widget_dispose()
// - demo_widget_finalize()
// - demo_widget_class_init()
// - demo_widget_new()
