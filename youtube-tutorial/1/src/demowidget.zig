const c = @cImport(@cInclude("demowidget.h"));

fn alignPtrCast(comptime T: type, ptr: anytype) T {
    return @ptrCast(T, @alignCast(@alignOf(T), ptr));
}

// All extern functions of interest defined in demowidgets.c.
extern fn demo_widget_get_button(self: *DemoWidget) *c.GtkWidget;
extern fn demo_widget_set_button(self: *DemoWidget, button: *c.GtkWidget) void;
extern fn demo_widget_new() *c.GtkWidget;

// This type corresponds to the C DemoWidget typedef struct with a few helper functions
// for canonical Zig.
const DemoWidget = opaque {
    const Self = @This();

    inline fn get_button(self: *Self) *c.GtkWidget {
        return demo_widget_get_button(self);
    }

    inline fn set_button(self: *Self, button: *c.GtkWidget) void {
        demo_widget_set_button(self, button);
    }
};

// this function is used to build the widget. Note that although the docs refer
// to something as the 'instance_init' function, the subroutine is actually
// named *_init and not *_instance_init. That confused me at first.
export fn demo_widget_init(self: *DemoWidget) void {
    const self_widget = alignPtrCast(*c.GtkWidget, self);
    const optional_button = c.gtk_button_new_with_label("Hello, from Zig!");
    if (optional_button) |button| {
        self.set_button(button);
        c.gtk_widget_set_hexpand(button, @boolToInt(true));
        c.gtk_widget_set_parent(button, self_widget);
    }
}

pub inline fn new() *c.GtkWidget {
    return demo_widget_new();
}
