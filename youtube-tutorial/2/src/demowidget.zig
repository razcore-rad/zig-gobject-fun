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
    extern fn demo_widget_get_parent_class() c.gpointer;
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

    export fn demo_widget_init(self: *Self) void {
        const self_widget = alignPtrCast(*c.GtkWidget, self);
        const optional_button = c.gtk_button_new_with_label("Hello, from Zig!");
        if (optional_button) |button| {
            self.setButton(button);
            c.gtk_widget_set_hexpand(button, 1);
            c.gtk_widget_set_parent(button, self_widget);
        }
    }

    fn dispose(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            const self = @ptrCast(*Self, object);
            var button = self.getButton();

            c.g_clear_pointer(
                @ptrCast(*c.gpointer, &button),
                @ptrCast(c.GDestroyNotify, &c.gtk_widget_unparent),
            );

            if (alignPtrCast(*c.GObjectClass, getParentClass()).dispose) |parent_dispose| {
                parent_dispose(object);
            }
        }
    }

    fn finalize(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            if (alignPtrCast(*c.GObjectClass, getParentClass()).finalize) |parent_finalize| {
                parent_finalize(object);
            }
        }
    }

    export fn demo_widget_class_init(class: *c.DemoWidgetClass) void {
        const object_class = alignPtrCast(*c.GObjectClass, class);
        const default_flags = c.G_PARAM_READWRITE | c.G_PARAM_STATIC_STRINGS | c.G_PARAM_EXPLICIT_NOTIFY;

        object_class.dispose = &dispose;
        object_class.finalize = &finalize;
        object_class.set_property = &setProperty;
        object_class.get_property = &getProperty;

        properties[@enumToInt(Properties.prop_label)] = c.g_param_spec_string(
            "label",
            "Button label",
            "Textual label for our lovely button",
            "Hello, world!", // default
            default_flags,
        );
        c.g_object_class_install_properties(object_class, @enumToInt(Properties.n_properties), &properties);

        const widget_class = alignPtrCast(*c.GtkWidgetClass, class);
        c.gtk_widget_class_set_layout_manager_type(widget_class, c.gtk_box_layout_get_type());
    }

    pub fn new() ?*c.GtkWidget {
        return alignPtrCast(?*c.GtkWidget, c.g_object_new(getType(), null));
    }
};
