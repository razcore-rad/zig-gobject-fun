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

const Signals = enum(c.guint) {
    toggled,
    n_signals,
};

var signals: [@enumToInt(Signals.n_signals)]c.guint = undefined;

// This type corresponds to the C DemoWidget typedef struct with a few helper functions
// for canonical Zig.
pub const DemoWidget = opaque {
    const Self = @This();

    // All extern functions of interest defined in demowidgets.c.
    extern fn demo_widget_get_parent_class() c.gpointer;
    const get_parent_class = demo_widget_get_parent_class;

    extern fn demo_widget_get_label(self: *Self) *u8;
    extern fn demo_widget_set_label(self: *Self, label: *u8) void;
    const get_label = demo_widget_get_label;
    const set_label = demo_widget_set_label;

    extern fn demo_widget_get_layout(self: *Self) *c.PangoLayout;
    extern fn demo_widget_set_layout(self: *Self, layout: ?*c.PangoLayout) void;
    const get_layout = demo_widget_get_layout;
    const set_layout = demo_widget_set_layout;

    extern fn demo_widget_get_layout_rect(self: *Self) c.GdkRectangle;
    extern fn demo_widget_set_layout_rect(self: *Self, layout_rect: c.GdkRectangle) void;
    const get_layout_rect = demo_widget_get_layout_rect;
    const set_layout_rect = demo_widget_set_layout_rect;

    extern fn demo_widget_get_label_selected(self: *Self) c.gboolean;
    extern fn demo_widget_set_label_selected(self: *Self, label_selected: c.gboolean) void;
    const get_label_selected = demo_widget_get_label_selected;
    const set_label_selected = demo_widget_set_label_selected;

    extern fn demo_widget_get_gesture(self: *Self) *c.GtkGesture;
    extern fn demo_widget_set_gesture(self: *Self, gesture: ?*c.GtkGesture) void;
    const get_gesture = demo_widget_get_gesture;
    const set_gesture = demo_widget_set_gesture;

    // We can't directly use the GTK4-way c.DEMO_TYPE_WIDGET macro.
    extern fn demo_widget_get_type() c.GType;
    const get_type = demo_widget_get_type;

    pub fn setLabel(self: *Self, new_label: [*:0]const u8) void {
        var label_width: c_int = 0;
        var label_height: c_int = 0;

        var label = self.get_label();
        c.g_clear_pointer(@ptrCast(*c.gpointer, &label), c.g_free);
        self.set_label(c.g_strdup(new_label));

        label = self.get_label();
        const layout = self.get_layout();
        c.pango_layout_set_text(layout, label, -1);
        c.pango_layout_get_size(layout, &label_width, &label_height);

        var layout_rect = self.get_layout_rect();
        layout_rect.width = @divTrunc(label_width, c.PANGO_SCALE);
        layout_rect.height = @divTrunc(label_height, c.PANGO_SCALE);

        c.gtk_widget_queue_draw(alignPtrCast(*c.GtkWidget, self));
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
            @enumToInt(Properties.prop_label) => c.g_value_set_string(value, self.get_label()),
            else => std.debug.print("Invalid property `{?any}[{}]` with spec {?any}", .{ object, property_id, pspec }),
        }
    }

    fn renderHighlights(self: *Self, snap_shot: *c.GtkSnapshot, optional_context: ?*c.GtkStyleContext) void {
        const context = optional_context orelse return;
        const layout_rect = self.get_layout_rect();
        const grect = c.graphene_rect_t{
            .origin = .{
                .x = @intToFloat(f32, layout_rect.x),
                .y = @intToFloat(f32, layout_rect.y),
            },
            .size = .{
                .width = @intToFloat(f32, layout_rect.width),
                .height = @intToFloat(f32, layout_rect.height),
            },
        };

        c.gtk_style_context_save(context);
        c.gtk_style_context_set_state(context, c.GTK_STATE_FLAG_SELECTED);
        c.gtk_snapshot_push_clip(snap_shot, &grect);
        c.gtk_snapshot_render_background(snap_shot, context, @intToFloat(f64, layout_rect.x), @intToFloat(f64, layout_rect.y), c.graphene_rect_get_width(&grect), c.graphene_rect_get_height(&grect));
        c.gtk_snapshot_render_layout(snap_shot, context, @intToFloat(f64, layout_rect.x), @intToFloat(f64, layout_rect.y), self.get_layout());
        c.gtk_snapshot_pop(snap_shot);
        c.gtk_style_context_restore(context);
    }

    fn snapshot(optional_widget: ?*c.GtkWidget, optional_snapshot: ?*c.GtkSnapshot) callconv(.C) void {
        const widget = optional_widget orelse return;
        const snap_shot = optional_snapshot orelse return;
        const self = alignPtrCast(*Self, widget);

        const context = c.gtk_widget_get_style_context(widget);
        c.gtk_style_context_add_class(context, "view");

        const provider = c.gtk_css_provider_new();
        c.gtk_css_provider_load_from_data(provider,
            \\#demowidget {
            \\  border: 5px solid red;
            \\}
        , -1);

        c.gtk_style_context_add_provider(context, @ptrCast(*c.GtkStyleProvider, provider), c.GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        var allocation: c.GtkAllocation = undefined;
        c.gtk_widget_get_allocation(widget, &allocation);

        c.gtk_snapshot_render_background(
            snap_shot,
            context,
            @intToFloat(f64, allocation.x),
            @intToFloat(f64, allocation.y),
            @intToFloat(f64, allocation.width),
            @intToFloat(f64, allocation.height),
        );
        var layout = self.get_layout();
        c.gtk_snapshot_render_layout(snap_shot, context, @intToFloat(f64, allocation.x), @intToFloat(f64, allocation.y), layout);
        if (self.get_label_selected() != 0) {
            renderHighlights(self, snap_shot, context);
        }
    }

    fn toggleSelection(self: *Self) void {
        self.set_label_selected(if (self.get_label_selected() != 0) 0 else 1);
        c.gtk_widget_queue_draw(alignPtrCast(*c.GtkWidget, self));
        c.g_signal_emit(self, signals[@enumToInt(Signals.toggled)], 0);
    }

    fn gestureReleasedCb(optional_gesture: ?*c.GtkGestureClick, n_press: c_int, x: f32, y: f32, optional_user_data: c.gpointer) void {
        _ = n_press;
        _ = optional_gesture;
        const user_data = optional_user_data orelse return;
        const self = alignPtrCast(*Self, user_data);
        const layout_rect = self.get_layout_rect();
        if (x <= @intToFloat(f32, layout_rect.width) and y <= @intToFloat(f32, layout_rect.height)) {
            self.toggleSelection();
        }
    }

    pub fn toggledCb(demo_widget: *Self, user_data: c.gpointer) void {
        _ = user_data;
        _ = demo_widget;
        std.debug.print("DemoWidget says hello.\n", .{});
    }

    export fn demo_widget_init(self: *Self) void {
        const self_widget = alignPtrCast(*c.GtkWidget, self);
        c.gtk_widget_set_name(self_widget, "demowidget");
        self.set_layout(c.gtk_widget_create_pango_layout(self_widget, null));

        const gesture = c.gtk_gesture_click_new();
        self.set_gesture(gesture);

        _ = c.g_signal_connect_data(gesture, "released", @ptrCast(c.GCallback, &gestureReleasedCb), self, null, c.G_CONNECT_DEFAULT);
        c.gtk_widget_add_controller(self_widget, @ptrCast(*c.GtkEventController, gesture));
    }

    fn dispose(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            if (alignPtrCast(*c.GObjectClass, get_parent_class()).dispose) |parent_dispose| {
                parent_dispose(object);
            }
        }
    }

    fn finalize(optional_object: ?*c.GObject) callconv(.C) void {
        if (optional_object) |object| {
            if (alignPtrCast(*c.GObjectClass, get_parent_class()).finalize) |parent_finalize| {
                const self = alignPtrCast(*Self, object);
                var label = self.get_label();
                c.g_clear_pointer(@ptrCast(*c.gpointer, &label), c.g_free);
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

        const widget_class = alignPtrCast(*c.GtkWidgetClass, class);
        widget_class.snapshot = &snapshot;

        properties[@enumToInt(Properties.prop_label)] = c.g_param_spec_string(
            "label",
            "Button label",
            "Textual label for our lovely button",
            "Hello, world!", // default
            default_flags,
        );
        c.g_object_class_install_properties(object_class, @enumToInt(Properties.n_properties), &properties);

        signals[@enumToInt(Signals.toggled)] = c.g_signal_new_class_handler(
            "toggled",
            c.G_OBJECT_CLASS_TYPE(object_class),
            c.G_SIGNAL_RUN_LAST,
            null,
            null,
            null,
            null,
            c.G_TYPE_NONE,
            0,
        );

        c.gtk_widget_class_set_layout_manager_type(widget_class, c.gtk_box_layout_get_type());
    }

    pub fn new() ?*c.GtkWidget {
        return alignPtrCast(?*c.GtkWidget, c.g_object_new(get_type(), null));
    }
};
