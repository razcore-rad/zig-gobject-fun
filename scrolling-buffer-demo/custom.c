#include "custom.h"

/* touch */
#define TOTAL_LINES_NUM	100

/* do not touch */
#define TOTAL_LINES	((unsigned int)TOTAL_LINES_NUM)

struct _DemoWidget
{
	GtkDrawingArea parent_instance;

	GtkAdjustment *vadjustment;
	GtkEventController *scroll_event_controller;
	GQueue *queue;
};

G_DEFINE_TYPE(DemoWidget, demo_widget, GTK_TYPE_DRAWING_AREA)

/* PRIVATE DATA */

/* data for draw_cb helpers. This is mostly just here because I'm lazy. */
typedef struct {
	DemoWidget *self;
	cairo_t *cr;
	GtkStyleContext *context;
	PangoLayout *layout;
	double width, height;
	int char_height;
	unsigned int lines_per_screen;
	double vadj_value;
} draw_data;

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

/* draw_cb helper functions */
static void
draw_render_vscroll(draw_data *data)
{
	double factor = -(data->vadj_value) * data->char_height;

	cairo_translate(data->cr, 0, factor);
}

static void
draw_background(draw_data *data)
{
	gtk_render_background(data->context, data->cr,
			0,		// gdouble x,
			0,		// gdouble y,
			data->width,	// gdouble width,
			data->height);	// gdouble height);
}

static void
draw_visible_lines(draw_data *data)
{
	DemoWidget *self = data->self;

	g_debug("%s: lines_per_screen: %u",
					__func__, data->lines_per_screen);

	for (unsigned int i = 0;
		i < MIN(g_queue_get_length(self->queue),
			data->lines_per_screen);
		++i)
	{
		PangoLayout *layout;
		
		layout = g_queue_peek_nth(self->queue, i);
		g_return_if_fail(PANGO_IS_LAYOUT(layout));

		gtk_render_layout(data->context, data->cr,
				0,						// gdouble x,
				i * data->char_height,	// gdouble y,
				layout);
	}
}

static void
draw_render_next_line(draw_data *data)
{
	DemoWidget *self = data->self;
	unsigned int qlength;
	unsigned int threshold;

	qlength = g_queue_get_length(self->queue);

	if (qlength <= data->lines_per_screen) {
			g_debug("%s: num of lines to render is less than or equal to "
					"the number of lines per screen. No need to draw more.",
							__func__);
			return;
	}

	/* nb: we *want* the implicit conversion to an integer here. */
	threshold = ABS(data->vadj_value);
	/* peg it to the maximum index we'd ever want. */
	threshold = MIN(threshold,
					/* -1 because we want an *index* (0 counts as a number) */
					qlength - data->lines_per_screen - 1);

	g_debug("%s: num: threshold: %u",
					__func__, threshold);

	for (unsigned int count = 0; count <= threshold; ++count)
	{
		PangoLayout *layout;

		layout = g_queue_peek_nth(self->queue,
									data->lines_per_screen + count);
		g_return_if_fail(PANGO_IS_LAYOUT(layout));

		gtk_render_layout(data->context, data->cr,
				0,									// gdouble x,
				(data->lines_per_screen + count) *
					data->char_height,				// gdouble y
				layout);
	}
}

static void
draw_update_adjustment(draw_data *data)
{
	DemoWidget *self = data->self;

	gtk_adjustment_configure(self->vadjustment,
		/* gdouble value -- don't change here; that's done in the 
		 * scroll_cb function. */
			data->vadj_value,
		/* gdouble lower */
			0.0,
		/* gdouble upper */
			TOTAL_LINES,
		/* gdouble step_increment, */
			1.0,
		/* gdouble page_increment, */
			data->lines_per_screen - 1,
		/* gdouble page_size); */
			data->lines_per_screen);
}

static gboolean
draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	draw_data data;

	(void)user_data;	/* unused for now */

	/* fill up data struct for helper functions. */
	data.self = DEMO_WIDGET(widget);
	data.cr = cr;
	data.context = gtk_widget_get_style_context(widget);
	data.layout = gtk_widget_create_pango_layout(widget, NULL);
	data.width = gtk_widget_get_allocated_width(widget);
	data.height = gtk_widget_get_allocated_height(widget);
	data.vadj_value = gtk_adjustment_get_value(data.self->vadjustment);
	data.char_height = get_character_height(data.layout);
	data.lines_per_screen = data.height / data.char_height;

	/* helpers */
	draw_render_vscroll(&data);
	draw_background(&data);
	draw_visible_lines(&data);
	draw_render_next_line(&data);
	draw_update_adjustment(&data);

	return GDK_EVENT_PROPAGATE;
}

/* CONSTRUCTORS AND DESTRUCTORS */
	
static void
demo_widget_init(DemoWidget *self)
{
	GtkWidget *widget = GTK_WIDGET(self);
	GtkStyleContext *context = gtk_widget_get_style_context(widget);

	/* initial setup */
	gtk_widget_set_can_focus(widget, TRUE);

	/* use the `.view' style class utilized by e.g. GtkTextView so we can
	 * get theming for pseudo-classes like :selected for free. */
	gtk_style_context_add_class(context, GTK_STYLE_CLASS_VIEW);

	/* queue of PangoLayouts */
	self->queue = g_queue_new();
	for (unsigned int i = 0; i < TOTAL_LINES; ++i)
	{
		PangoLayout *layout;
		char *str;

		str = g_strdup_printf("%d: foo bar baz", i + 1);
		layout = gtk_widget_create_pango_layout(widget, str);

		g_queue_push_tail(self->queue, layout);
		g_free(str);
	}

	/* adjustment */
	self->vadjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	/* scroll event controller */
	self->scroll_event_controller =
		gtk_event_controller_scroll_new(widget,
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

	/* nothing here for now. */
	(void)self;

	/* Boilerplate.
	 * this is called `chaining up' by the gobject docs. *_parent_class
	 * is auto-generated by the macros - acc. to gtype.h, it's a static
	 * variable pointing to the parent class.
	 */
	G_OBJECT_CLASS(demo_widget_parent_class)->dispose(object);
}

/* another stage of the destruction process. My understanding is that here, you
 * free items that are independent only; for things that are ref-counted,
 * use dispose.
 *
 * See:
 * https://developer.gnome.org/gobject/stable/howto-gobject-destruction.html
 * for a description of the nuances.
 */
static void
demo_widget_finalize(GObject *gobject)
{
	DemoWidget *self = DEMO_WIDGET(gobject);

	/* dispose queue of PangoLayouts. This can't be done safely in _dispose
	 * as g_queue_free_full can't take a function that takes a double
	 * pointer to pass g_clear_object */
	g_queue_free_full(self->queue, g_object_unref);

	/* Boilerplate:
	 * Always chain up to the parent class; as with dispose(), finalize()
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

// vim: colorcolumn=80:tabstop=4:shiftwidth=4
