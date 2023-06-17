# Zig - GTK4 Exploration

> [The original C repo](https://gitlab.com/LARathbone/gobject-fun)

> The Zig-implementation lives in `youtube-tutorial`.

[Zig](https://ziglang.org) has the capability to [translate C code to Zig](https://ziglang.org/documentation/master/#toc-C-Translation-CLI) code using a CLI tool via `zig translate-c` or in code with the built-in function `@cImport()`.

Unfortunately Zig isn't capable of translating complex C code that heavily relies on macros like [GTK4](https://gtk.org). Because of this, we aren't able to create custom widgets in a simple fashion.

The compromise I'm using is to create the boilerplate code in C while implementing the "heavy-lifting" methods in Zig.

## Status

Zig data types carry extra information (like alignment) compared to C so it isn't trivial to use C pointer types directly in Zig. A lot of explicit casting has to take place, which in fairness happens in C as well, but the GTK4 macros masks out most of this noise.

That said, the idea is to convert as much C code to Zig while retaining sanity:

- [x] `youtube-tutorial/1`. The custom method (if we stick with the OOP metaphor) implementations are moved to `demowidget.zig`. These are all under an opaque type called `DemoWidget` exposing a canonical Zig function-calling scheme.

  - `demo_widget_init()`.
  - `demo_widget_dispose()`.
  - `demo_widget_finalize()`.
  - `demo_widget_class_init()`.
  - A public "ziggified" version of `demo_widget_new()` to `DemoWidget.new()`.
  - Helper "ziggified" functions for internal use: `DemoWidget.get_parent_class()`, `DemoWidget.get_button()`, `DemoWidget.set_button()`. These are required because `DemoWidget` is an opaque type, and we can't access fields directly. The setter/getter implementation is on the C side in `demowidget.c`.

- [ ] `youtube-tutorial/2`
- [ ] `youtube-tutorial/3`
- [ ] `youtube-tutorial/4`
- [ ] `youtube-tutorial/5`
- [ ] `youtube-tutorial/6`

## Building

Go into any `youtube-tutorial/*` subfolder and execute `zig build run` to build and run the app.

### Dependencies

- `libgtk-4-dev`

## Acknowledgements

This work is not possible without [the GTK4 tutorial by Logan Rathbone](https://www.youtube.com/playlist?list=PLFnRB0VoVErpBiFZlMg_JAPKdJJobzyV3) and [atoft's Reactions](https://codeberg.org/atoft/Reactions) GIF search app that put me on the right track to understanding how to interoperate with GTK4 in C.
