const std = @import("std");
const DemoWidget = @import("demowidget.zig").DemoWidget;
const c = @cImport(@cInclude("demowidget.h"));

fn activateCb(app: *c.GtkApplication, user_data: ?*anyopaque) void {
    _ = user_data;

    if (c.gtk_application_window_new(app)) |window_widget| {
        const window = @ptrCast(*c.GtkWindow, window_widget);
        c.gtk_window_set_title(window, "Window");
        c.gtk_window_set_default_size(window, 400, 400);

        if (DemoWidget.new()) |demo_widget| {
            c.gtk_window_set_child(window, demo_widget);
            c.gtk_window_present(window);
        }
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
