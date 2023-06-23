// TODO: clean-up code and better align with C pointers
const std = @import("std");
pub const c = @cImport(@cInclude("demowidget.h"));

fn alignPtrCast(comptime T: type, ptr: anytype) T {
    return @ptrCast(T, @alignCast(@alignOf(T), ptr));
}

const Properties = enum(c.guint) {
    prop_label = 1,
    n_properties,
};

var properties: [@enumToInt(Properties.n_properties)]?*c.GParamSpec = undefined;

// This type corresponds to the C DemoWidget typedef struct with a few helper functions
// for canonical Zig.
pub const DemoWidget = opaque {
    const Self = @This();

    // All extern functions of interest defined in demowidgets.c.
    extern fn demo_widget_get_parent_class() ?*anyopaque;
    const getParentClass = demo_widget_get_parent_class;

    extern fn demo_widget_get_button(self: *Self) *c.GtkWidget;
    const getButton = demo_widget_get_button;

    extern fn demo_widget_set_button(self: *Self, button: *c.GtkWidget) void;
    const setButton = demo_widget_set_button;

    // We can't directly use the GTK4-way c.DEMO_TYPE_WIDGET macro.
    extern fn demo_widget_get_type() c.GType;
    const getType = demo_widget_get_type;

    pub fn setLabel(self: *Self, label: [*:0]const u8) void {
        c.gtk_button_set_label(@ptrCast(*c.GtkButton, self.getButton()), label);
        c.g_object_notify_by_pspec(alignPtrCast(*c.GObject, self), properties[@enumToInt(Properties.prop_label)]);
    }

    fn setProperty(object: ?*c.GObject, property_id: c.guint, value: ?*const c.GValue, pspec: ?*c.GParamSpec) callconv(.C) void {
        const self = alignPtrCast(*Self, object);

        switch (property_id) {
            @enumToInt(Properties.prop_label) => setLabel(self, c.g_value_get_string(value)),
            else => std.debug.print("Invalid property `{?any}[{}]` with spec {?any}", .{ object, property_id, pspec }),
        }
    }

    fn getProperty(object: ?*c.GObject, property_id: c.guint, value: ?*c.GValue, pspec: ?*c.GParamSpec) callconv(.C) void {
        const self = alignPtrCast(*Self, object);

        switch (property_id) {
            @enumToInt(Properties.prop_label) => c.g_value_set_string(value, c.gtk_button_get_label(@ptrCast(*c.GtkButton, self.getButton()))),
            else => std.debug.print("Invalid property `{?any}[{}]` with spec {?any}", .{ object, property_id, pspec }),
        }
    }

    // this function is used to build the widget. Note that although the docs refer
    // to something as the 'instance_init' function, the subroutine is actually
    // named *_init and not *_instance_init. That confused me at first.
    export fn demo_widget_init(self: *Self) void {
        const self_widget = alignPtrCast(*c.GtkWidget, self);
        const optional_button = c.gtk_button_new_with_label("Hello, from Zig!");
        if (optional_button) |button| {
            self.setButton(button);
            c.gtk_widget_set_hexpand(button, 1);
            c.gtk_widget_set_parent(button, self_widget);
        }
    }

    // This is one stage of the destructor process along with _finalize.
    // Here, you mostly destruct items that are reference-counted (eg, GObject-y
    // stuff) as opposed to not (like a string, or a FILE * pointer, which should
    // be freed / fclosed as opposed to being unreffed).
    // It is important to remember this method may be called multiple times. It
    // is a programmer error not to account for that. If you unref a GObject that
    // already has a reference count of 0, you'll get a runtime error. See below
    // for how we account for that.
    // See Part 1B of this series for problems you can run into if you don't deal
    // with this correctly.
    // See also:
    // https://developer-old.gnome.org/gobject/stable/howto-gobject-destruction.html
    fn dispose(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            const self = @ptrCast(*Self, object);
            var button = self.getButton();

            // g_clear_pointer is a very useful function. It calls a function of your
            // choice to unref or free a pointer, and then sets that pointer to NULL.
            // If it is called on a NULL pointer, it simply does nothing. That allows
            // for a perfect way to handle the issue of _dispose being called multiple
            // times.
            // Here, gtk_widget_unparent is the standard GTK function to de-couple
            // a child widget from its parent, which unrefs that widget behind the
            // scenes.
            // Note that g_clear_pointer takes a pointer *to* a gpointer, and not a
            // straight gpointer. That is necessary for the function to NULL-ify the
            // pointer.
            c.g_clear_pointer(
                @ptrCast(*?*anyopaque, &button),
                @ptrCast(c.GDestroyNotify, &c.gtk_widget_unparent),
            );

            // Final step: Chain up (boilerplate)
            // This is called `chaining up' by the gobject docs. *_parent_class
            // is auto-generated by the macros - acc. to gtype.h, it's a static
            // variable pointing to the parent class.
            if (alignPtrCast(*c.GObjectClass, getParentClass()).dispose) |parent_dispose| {
                parent_dispose(object);
            }
        }
    }

    // Second and final stage of the destruction process. See _finalize above for
    // more info. We don't have anything to put in here yet, but we include it for
    // posterity and for easy addition later; so we just manually chain up for now.
    fn finalize(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            //

            // Always chain up to the parent class; as with dispose(), finalize() is
            // autogenerated and thus always guaranteed to exist on the parent's class
            // virtual function table
            //
            if (alignPtrCast(*c.GObjectClass, getParentClass()).finalize) |parent_finalize| {
                parent_finalize(object);
            }
        }
    }

    //  this method gets run the first time the class is *ever* utilized but not
    //  again.
    //
    //  See:
    //  https://developer-old.gnome.org/gobject/stable/howto-gobject-destruction.html
    //  for more info on what you need to put in this subroutine re: destructors.
    //
    //  Note: I kept getting confused about what 'klass' meant and why it's spelled
    //  that way. It is just to avoid using the token 'class' on its own which is a
    //  c++ reserved word. You could call it something else like 'gclass' if you'd
    //  like, but it has idiomatically been called 'klass' for 20+ years, so it's
    //  probably best to just stick with that.
    export fn demo_widget_class_init(class: *c.DemoWidgetClass) void {
        const object_class = alignPtrCast(*c.GObjectClass, class);
        const default_flags = c.G_PARAM_READWRITE | c.G_PARAM_STATIC_STRINGS | c.G_PARAM_EXPLICIT_NOTIFY;

        object_class.dispose = &dispose;
        object_class.finalize = &finalize;
        object_class.set_property = &setProperty;
        object_class.get_property = &getProperty;

        properties[@enumToInt(Properties.prop_label)] = c.g_param_spec_string("label", "Button label", "Textual label for our lovely button", "Hello, world!", default_flags);
        c.g_object_class_install_properties(object_class, @enumToInt(Properties.n_properties), &properties);

        const widget_class = alignPtrCast(*c.GtkWidgetClass, class);
        c.gtk_widget_class_set_layout_manager_type(widget_class, c.gtk_box_layout_get_type());
    }

    // and finally, here's the actual definition of our public function to create
    // an instance of our object.
    //
    // It takes the GType of the GObject type, followed (optionally) by name/value
    // pairs to set object 'properties', followed by NULL when you're done. Since
    // we don't have any properties we're going to instantiate, we just pass NULL
    // after the GType. We'll discuss GObject properties later on.
    pub fn new() ?*c.GtkWidget {
        return alignPtrCast(?*c.GtkWidget, c.g_object_new(getType(), null));
    }
};
