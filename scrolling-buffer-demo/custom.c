#include "custom.h"

#define SCREENS_OF_TEXT	3

struct _DemoWidget
{
	GtkDrawingArea parent_instance;

	GtkAdjustment *vadjustment;
	GtkEventController *scroll_event_controller;
};

G_DEFINE_TYPE(DemoWidget, demo_widget, GTK_TYPE_DRAWING_AREA)

/* PRIVATE METHODS */

static int
get_character_height(PangoLayout *layout)
{
	int height = 0;
	PangoRectangle rect = { 0 };

	pango_layout_index_to_pos(layout,
			0,		// index
			&rect);

	height = rect.height / PANGO_SCALE;

	g_return_val_if_fail(height, 0);
	return height;
}

/* CALLBACKS */

static void
scroll_event_controller_scroll_cb(GtkEventControllerScroll *controller,
		double		dx,
		double		dy,
		gpointer	user_data)
{
	DemoWidget *self = DEMO_WIDGET(user_data);
	double value = gtk_adjustment_get_value(self->vadjustment);

	g_debug("%s: dx: %f - dy: %f - value: %f - value + dy: %f",
			__func__, dx, dy, value, value + dy);

	gtk_adjustment_set_value(self->vadjustment,
			value + dy);
}

static void
vadjustment_value_changed_cb(GtkAdjustment *adjustment,
		gpointer	user_data)
{
	DemoWidget *self = DEMO_WIDGET(user_data);

	/* queue a redraw to render scrolling. */
	gtk_widget_queue_draw(GTK_WIDGET(self));
}

static gboolean
draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	DemoWidget *self = DEMO_WIDGET(widget);
	GtkStyleContext *context;
	PangoLayout *layout;
	double width, height;
	int char_height;
	int lines_per_screen;
	double vadj_value;
	double vscroll_factor;
	int i, j;

	(void)data;

	context = gtk_widget_get_style_context(widget);
	layout = gtk_widget_create_pango_layout(widget, NULL);
	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	vadj_value = gtk_adjustment_get_value(self->vadjustment);

	g_debug("%s: width: %f - height: %f",
			__func__, width, height);

	pango_layout_set_text(layout, "foo bar baz", -1);

	char_height = get_character_height(layout);
	lines_per_screen = height / char_height;

	g_debug("char_height: %d", char_height);
	g_debug("lines_per_screen: %d", lines_per_screen);

	/* render vertical scrolling */
	vscroll_factor = -vadj_value * char_height;
	cairo_translate(cr, 0, vscroll_factor);
	g_debug("%s: vscroll_factor: %f",
			__func__, vscroll_factor);

	/* draw the background */
	gtk_render_background(context, cr,
			0,		// gdouble x,
			0,		// gdouble y,
			width,		// gdouble width,
			height);	// gdouble height);

	/* draw our lines of text */
	for (i = 0; i < lines_per_screen; ++i)
	{
		char *str = g_strdup_printf("%d: foo bar baz", i + 1);
		pango_layout_set_text(layout, str, -1);
		gtk_render_layout(context, cr,
				0,			// gdouble x,
				i * char_height,	// gdouble y,
				layout);
		g_free(str);
	}

	/* render one line ahead above and beyond the main screen. */
	j = 0;
	while (i + 1 <= gtk_adjustment_get_upper(self->vadjustment))
	{
		if (ABS(vscroll_factor) != 0 &&
				ABS(vscroll_factor) / j + char_height > j)
		{
			char *str = g_strdup_printf("%d: foo bar baz", i + 1);
			pango_layout_set_text(layout, str, -1);
			gtk_render_layout(context, cr,
					0,
					i * char_height,
					layout);
			g_free(str);
			++j;
		}
		++i;
	}

	gtk_adjustment_configure(self->vadjustment,
		/* gdouble value -- don't change here; that's done in the 
		 * scroll_cb function. */
			vadj_value,
		/* gdouble lower */
			0.0,
		/* gdouble upper */
			lines_per_screen * SCREENS_OF_TEXT,
		/* gdouble step_increment, */
			1.0,
		/* gdouble page_increment, */
			lines_per_screen - 1,
		/* gdouble page_size); */
			lines_per_screen);

	return GDK_EVENT_PROPAGATE;
}

/* CONSTRUCTORS AND DESTRUCTORS */

static void
demo_widget_init(DemoWidget *self)
{
	gtk_widget_set_can_focus(GTK_WIDGET(self), TRUE);

	/* adjustment */
	self->vadjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	/* scroll event controller */
	self->scroll_event_controller =
		gtk_event_controller_scroll_new(GTK_WIDGET(self),
				GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);

	/* SIGNALS */

	/* connect to draw signal */
	g_signal_connect(G_OBJECT(self), "draw",
			G_CALLBACK(draw_cb), NULL);

	/* vertical adjustment signal */
	g_signal_connect(G_OBJECT(self->vadjustment), "value-changed",
			G_CALLBACK(vadjustment_value_changed_cb), self);
	
	/* scroll event controller signal */
	g_signal_connect(G_OBJECT(self->scroll_event_controller), "scroll",
			G_CALLBACK(scroll_event_controller_scroll_cb), self);
}

static void
demo_widget_dispose(GObject *object)
{
	DemoWidget *self = DEMO_WIDGET(object);

	/* destructors go here */
	(void)self;

	/* Boilerplate.
	 * this is called `chaining up' by the gobject docs. *_parent_class
	 * is auto-generated by the macros - acc. to gtype.h, it's a static
	 * variable pointing to the parent class.
	 */
	G_OBJECT_CLASS(demo_widget_parent_class)->dispose(object);
}

/* another stage of the destruction process. My understanding is that here, you
 * free items that are independent only; for things that have a reference to
 * 'self', you use dispose.
 *
 * See:
 * https://developer.gnome.org/gobject/stable/howto-gobject-destruction.html
 * for a description of the nuances.
 */
static void
demo_widget_finalize(GObject *gobject)
{
	/* here, you would free stuff. I've got nuthin' for ya. */

	/* --- */

	/* Always chain up to the parent class; as with dispose(), finalize()
	 * is autogenerated and thus always guaranteed to exist on the parent's
	 * class virtual function table
	 */
	
	G_OBJECT_CLASS(demo_widget_parent_class)->finalize(gobject);
}

static void
demo_widget_class_init(DemoWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = demo_widget_dispose;
	object_class->finalize = demo_widget_finalize;
}

GtkWidget *
demo_widget_new(void)
{
	return g_object_new(DEMO_TYPE_WIDGET, NULL);
}

// vim: colorcolumn=80
