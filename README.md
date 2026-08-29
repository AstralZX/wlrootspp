# wlroots++

A modern **C++20 RAII wrapper** around the
[wlroots](https://gitlab.freedesktop.org/wlroots/wlroots) 0.20 Wayland
compositor library. It wraps wlroots' C ABI behind owning object types so a
compositor can be written in idiomatic C++, and it links in the
[scenefx](https://github.com/wlrfx/scenefx) effect engine (blur, shadows,
rounded corners) plus a small `wlr::fx` animation helper.

wlroots has no stable ABI across versions, so wlroots++ pins a target and
exposes the scene graph, surface tree, seat, output and XDG-shell plumbing as
RAII classes with `wl_signal` → `std::function` callbacks.

## License

**AGPL-3.0-only.** This is a deliberate strong-copyleft choice: any project that
links against or uses wlroots++ must itself be released under the AGPL-3.0. See
`COPYING`.

## Target

wlroots has no stable ABI across versions, so wlroots++ pins a target:

* **wlroots 0.20** (pkg-config name `wlroots-0.20`)
* **scenefx 0.5** (pkg-config name `scenefx-0.5`)

## Building

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Requires development files for: `wlroots-0.20`, `scenefx-0.5`, `wayland-server`,
`xkbcommon`, `pixman`. The library is compiled with `-DWLR_USE_UNSTABLE`, which
is propagated as a public definition to consumers.

This produces `libwlrootspp.a` plus the `scene_demo` and `tinywl` examples.

## Layout

```
include/wlr/wlr.hpp            C++20 compat shim + wlroots/scenefx includes
include/wlr/signals.hpp        SignalSink: wl_signal -> std::function<void(void*)>
include/wlr/display.hpp        Display (wl_display lifecycle, socket, run loop)
include/wlr/backend.hpp        Backend (autocreate, start, new_output/new_input)
include/wlr/compositor.hpp     Renderer, Allocator, DataDeviceManager, Compositor
include/wlr/output.hpp         Output, SceneOutput, OutputLayout
include/wlr/xdg_shell.hpp      XdgSurface, Toplevel, XdgShell
include/wlr/seat.hpp           Cursor, Keyboard, Seat
include/wlr/scene.hpp          Scene, Box, Color, CornerRadii
include/wlr/node.hpp           Node, Tree, Rect, Shadow, Blur, OptimizedBlur, Buffer, Surface
include/wlr/scenefx/anim.hpp   wlr::fx::Easing, wlr::fx::Animated<>
include/wlr/scenefx/effect.hpp wlr::fx::Effect
src/wlrootspp.cpp              version() and Node::as<T> conversions
examples/                     runnable demos (scene_demo, tinywl)
```

## Examples

* `examples/scene_demo.cpp` — exercises the scene graph and scenefx blur.
* `examples/tinywl.cpp` — a minimal C++ compositor: boots, creates a headless
  output, serves the full Wayland protocol (xdg_wm_base, wl_seat, wl_output,
  …), and maps XDG toplevels into the scene graph.

Run the compositor (headless, for testing):

```sh
WLR_BACKENDS=headless WLR_HEADLESS_OUTPUTS=1 ./build/examples/tinywl
# prints: tinywl: WAYLAND_DISPLAY=wayland-N
WAYLAND_DISPLAY=wayland-N your-wayland-client
```

## Usage

```cpp
#include "wlr/display.hpp"
#include "wlr/backend.hpp"
#include "wlr/compositor.hpp"
#include "wlr/output.hpp"
#include "wlr/xdg_shell.hpp"
#include "wlr/scene.hpp"

int main() {
	wlr::Display display;
	wlr::Backend backend(display);
	wlr::Renderer renderer(wlr_renderer_autocreate(backend.raw()));
	renderer.init_wl_display(display);
	wlr::Allocator alloc(wlr_allocator_autocreate(backend.raw(), renderer.raw()));
	wlr::Compositor comp(display, renderer);
	wlr::XdgShell xdg(display);
	wlr::OutputLayout layout(display);
	wlr::Scene scene;

	backend.on_new_output([&](void* data) {
		auto* o = static_cast<struct wlr_output*>(data);
		wlr_output_init_render(o, alloc.raw(), renderer.raw());
		wlr_scene_output_create(scene.raw(), o);
		layout.add_auto_output(o);
	});

	xdg.on_new_toplevel([&](void* data) {
		auto* t = static_cast<struct wlr_xdg_toplevel*>(data);
		wlr_scene_xdg_surface_create(scene.root(), t->base);
		wlr_xdg_surface_schedule_configure(t->base);
	});

	display.add_socket_auto();
	backend.start();
	display.run();
}
```

## Effects

`wlr::Scene` exposes scenefx blur controls directly:

```cpp
wlr::Scene scene;
scene.set_blur_radius(14);
scene.set_blur_num_passes(2);
scene.set_blur_brightness(1.0f);
```

Per-node corner radii, shadows and blur are available through the `Rect`,
`Shadow`, `Blur` and `Buffer` node wrappers in `wlr/node.hpp`.

## Animation helper

`wlr::fx` provides easing and an `Animated<T>` value plus an `Effect` aggregator
for driving per-node properties over time:

```cpp
wlr::fx::Effect fx;
fx.opacity().set_duration(300);
fx.opacity().set_easing(wlr::fx::Easing::InOutQuad);
fx.opacity().set(0.3f);
for (std::uint64_t t = 0; fx.animating(); t += 16) {
	fx.on_frame(t);
	fx.apply_to(node);
}
```
