// SPDX-License-Identifier: AGPL-3.0-only
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <xkbcommon/xkbcommon.h>

#include <wayland-server-core.h>

#include "wlr/display.hpp"
#include "wlr/backend.hpp"
#include "wlr/compositor.hpp"
#include "wlr/output.hpp"
#include "wlr/xdg_shell.hpp"
#include "wlr/seat.hpp"
#include "wlr/scene.hpp"
#include "wlr/node.hpp"

struct FrameScheduler {
	wl_event_source* source = nullptr;
	struct wlr_scene* scene = nullptr;
	std::vector<struct wlr_scene_output*>* outputs = nullptr;
};

static void commit_outputs(FrameScheduler* s) {
	for (struct wlr_scene_output* so : *s->outputs)
		wlr_scene_output_commit(so, nullptr);
}

static int frame_tick(void* data) {
	FrameScheduler* s = static_cast<FrameScheduler*>(data);
	commit_outputs(s);
	wl_event_source_timer_update(s->source, 16);
	return 0;
}

struct ToplevelState {
	wlr::XdgSurface xdg;
	wlr::Tree scene_tree;
	bool mapped = false;
	std::string title;
};

int main() {
	wlr::Display display;
	if (!display)
		return 1;
	const char* socket = display.add_socket_auto();
	if (!socket)
		return 1;
	fprintf(stderr, "tinywl: WAYLAND_DISPLAY=%s\n", socket);
	fflush(stderr);

	wlr::Backend backend(display);
	if (!backend)
		return 1;

	wlr::Renderer renderer(wlr_renderer_autocreate(backend.raw()));
	if (!renderer)
		return 1;
	if (!renderer.init_wl_display(display))
		return 1;

	wlr::Allocator allocator(wlr_allocator_autocreate(backend.raw(), renderer.raw()));
	if (!allocator)
		return 1;

	wlr::Compositor compositor(display, renderer);
	if (!compositor)
		return 1;

	wlr::XdgShell xdg_shell(display);
	if (!xdg_shell)
		return 1;

	wlr::DataDeviceManager data_device_manager(wlr_data_device_manager_create(display.raw()));
	(void)data_device_manager;

	wlr::Seat seat(display, "seat0");
	if (!seat)
		return 1;

	wlr::Cursor cursor;
	if (!cursor)
		return 1;

	wlr::OutputLayout layout(display);
	if (!layout)
		return 1;
	cursor.attach_output_layout(layout.raw());

	wlr::Scene scene;
	scene.set_blur_radius(14);
	scene.set_blur_num_passes(2);
	scene.set_blur_noise(0.0f);
	scene.set_blur_brightness(1.0f);
	scene.set_blur_contrast(1.0f);
	scene.set_blur_saturation(1.0f);

	wlr::Tree toplevels(wlr_scene_tree_create(scene.root()));
	if (!toplevels)
		return 1;

	std::vector<struct wlr_scene_output*> outputs;
	std::vector<ToplevelState*> toplevel_nodes;

	FrameScheduler scheduler;
	scheduler.scene = scene.raw();
	scheduler.outputs = &outputs;
	scheduler.source = wl_event_loop_add_timer(display.event_loop(), frame_tick, &scheduler);

	backend.on_new_output([&](void* data) {
		struct wlr_output* o = static_cast<struct wlr_output*>(data);
		wlr::Output out(o);
		out.set_preferred_mode();
		wlr_output_init_render(o, allocator.raw(), renderer.raw());
		wlr_output_create_global(o, display.raw());

		struct wlr_scene_output* so = wlr_scene_output_create(scene.raw(), o);
		wlr_scene_output_set_position(so, 0, 0);
		outputs.push_back(so);
		layout.add_auto_output(o);
		wl_event_source_timer_update(scheduler.source, 0);
	});

	backend.on_new_input([&](void* data) {
		struct wlr_input_device* device = static_cast<struct wlr_input_device*>(data);
		cursor.attach_input_device(device);
		if (device->type != WLR_INPUT_DEVICE_KEYBOARD)
			return;
		struct wlr_keyboard* kb = wlr_keyboard_from_input_device(device);
		wlr::Keyboard k(kb);
		k.set_default_keymap();
		seat.set_keyboard(k);
		seat.set_capabilities(WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
	});

	int toplevel_count = 0;
	xdg_shell.on_new_toplevel([&](void* data) {
		struct wlr_xdg_toplevel* t = static_cast<struct wlr_xdg_toplevel*>(data);
		wl_event_loop_add_idle(display.event_loop(),
			[](void* data) {
				auto* s = static_cast<struct wlr_xdg_surface*>(data);
				if (s->initialized)
					wlr_xdg_surface_schedule_configure(s);
			}, t->base);
		ToplevelState* tn = new ToplevelState{
			wlr::XdgSurface{t->base},
			wlr::Tree(wlr_scene_xdg_surface_create(toplevels.raw(), t->base)),
		};
		toplevel_nodes.push_back(tn);

		tn->xdg.on_map([&, tn](void*) {
			struct wlr_box lb = layout.box();
			int w = lb.width > 80 ? lb.width - 80 : 40;
			int h = lb.height > 80 ? lb.height - 80 : 40;
			int x = lb.x + 40 + (toplevel_count * 24) % w;
			int y = lb.y + 40 + (toplevel_count * 24) % h;
			toplevel_count++;
			tn->scene_tree.set_position(x, y);
			tn->scene_tree.raise_to_top();
			tn->mapped = true;
		});
		tn->xdg.on_unmap([tn](void*) {
			tn->mapped = false;
		});
		tn->xdg.on_destroy([&, tn](void*) {
			if (tn->scene_tree)
				wlr_scene_node_destroy(&tn->scene_tree.raw()->node);
			auto it = std::find(toplevel_nodes.begin(), toplevel_nodes.end(), tn);
			if (it != toplevel_nodes.end())
				toplevel_nodes.erase(it);
			delete tn;
		});

		wlr::Toplevel toplevel(t);
		toplevel.on_set_title([tn](void* d) {
			const char* s = static_cast<struct wlr_xdg_toplevel*>(d)->title;
			tn->title = s ? s : "";
		});
		toplevel.on_set_app_id([](void*) {});
		toplevel.on_request_maximize([&, tn, t](void* d) {
			auto* e = static_cast<struct wlr_xdg_toplevel_requested*>(d);
			wlr::Toplevel{t}.set_maximized(e->maximized);
			struct wlr_box lb = layout.box();
			tn->scene_tree.set_position(lb.x, lb.y);
			tn->scene_tree.set_enabled(true);
		});
		toplevel.on_request_minimize([&, tn, t](void* d) {
			auto* e = static_cast<struct wlr_xdg_toplevel_requested*>(d);
			tn->scene_tree.set_enabled(!e->minimized);
		});
		toplevel.on_request_fullscreen([&, tn, t](void* d) {
			auto* e = static_cast<struct wlr_xdg_toplevel_requested*>(d);
			wlr::Toplevel{t}.set_maximized(e->fullscreen);
			struct wlr_box lb = layout.box();
			tn->scene_tree.set_position(lb.x, lb.y);
			tn->scene_tree.set_enabled(true);
		});
		toplevel.on_request_move([](void*) {});
		toplevel.on_request_resize([](void*) {});
	});

	if (!backend.start())
		return 1;

	display.run();

	for (ToplevelState* tn : toplevel_nodes)
		delete tn;
	return 0;
}
