const std = @import("std");
const DemoWidget = @import("demowidget.zig").DemoWidget;
const c = @cImport(@cInclude("demowidget.h"));

fn activateCb(app: *c.GtkApplication, user_data: c.gpointer) void {
    _ = user_data;

    if (c.gtk_application_window_new(app)) |window_widget| {
        const window = @ptrCast(*c.GtkWindow, window_widget);

        const box_widget = c.gtk_box_new(c.GTK_ORIENTATION_HORIZONTAL, 6);
        const box = @ptrCast(*c.GtkBox, box_widget);
        c.gtk_box_set_homogeneous(box, 1);

        const button = c.gtk_button_new_with_label("Yo");

        const demo_widget = DemoWidget.new();
        c.gtk_widget_set_hexpand(demo_widget, 1);

        c.gtk_box_append(box, demo_widget);
        c.gtk_box_append(box, button);

        c.g_object_set(demo_widget, "label", "foo bar baz", c.NULL);
        _ = c.g_signal_connect_data(demo_widget, "toggled", @ptrCast(c.GCallback, &DemoWidget.toggledCb), null, null, c.G_CONNECT_DEFAULT);
        _ = c.g_signal_connect_data(button, "clicked", @ptrCast(c.GCallback, &DemoWidget.toggledCb), demo_widget, null, c.G_CONNECT_SWAPPED);

        c.gtk_window_set_title(window, "Window");
        c.gtk_window_set_default_size(window, 400, 400);
        c.gtk_window_set_child(window, box_widget);
        c.gtk_window_present(window);
    }
}

pub fn main() void {
    if (c.gtk_application_new("org.gtk.example", c.G_APPLICATION_DEFAULT_FLAGS)) |app| {
        defer c.g_object_unref(app);
        _ = c.g_signal_connect_data(app, "activate", @ptrCast(c.GCallback, &activateCb), null, null, c.G_CONNECT_DEFAULT);
        const c_args_count = @intCast(c_int, std.os.argv.len);
        const c_args = @ptrCast([*]?[*:0]u8, std.os.argv.ptr);
        const status = c.g_application_run(@ptrCast(*c.GApplication, app), c_args_count, c_args);
        std.process.exit(@intCast(u8, status));
    }
}
