# Zig - GTK4 Exploration

> [The original C repo](https://gitlab.com/LARathbone/gobject-fun).

[Zig](https://ziglang.org) has the capability to [translate C code to Zig](https://ziglang.org/documentation/master/#toc-C-Translation-CLI) code using a CLI tool via `zig translate-c` or in code with the built-in function `@cImport()`.

Unfortunately Zig isn't capable of translating complex C code that heavily relies on macros like [GTK4](https://gtk.org). Because of this, we aren't able to create custom widgets in a simple fashion.

The compromise I'm using is to create the boilerplate code in C that exposes the `*_init()` function to Zig so we can work on the implementation in Zig. These are then used as object files we add to the compiler.

## Acknowledgements

This work is not possible without [the GTK4 tutorial by Logan Rathbone](https://www.youtube.com/playlist?list=PLFnRB0VoVErpBiFZlMg_JAPKdJJobzyV3) and [atoft's Reactions](https://codeberg.org/atoft/Reactions) GIF search app that put me on the right track to understanding how to interoperate with GTK4 in C.
