// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <functional>

#include "wlr/wlr.hpp"
#include "wlr/signals.hpp"
#include "wlr/display.hpp"
#include "wlr/scene.hpp"

namespace wlr {

class Output {
public:
	Output(struct wlr_output* o) : m_output(o) {}
	struct wlr_output* raw() const { return m_output; }
	explicit operator bool() const { return m_output != nullptr; }
	const char* name() const { return m_output->name; }
	const char* make() const { return m_output->make; }
	const char* model() const { return m_output->model; }

	struct wlr_output_mode* preferred_mode() const {
		return wlr_output_preferred_mode(m_output);
	}
	bool set_preferred_mode() {
		struct wlr_output_mode* mode = wlr_output_preferred_mode(m_output);
		if (!mode) return false;
		struct wlr_output_state state;
		wlr_output_state_init(&state);
		wlr_output_state_set_mode(&state, mode);
		bool ok = wlr_output_commit_state(m_output, &state);
		wlr_output_state_finish(&state);
		return ok;
	}

private:
	struct wlr_output* m_output;
};

class SceneOutput {
public:
	SceneOutput(struct wlr_scene_output* so) : m_so(so) {}
	struct wlr_scene_output* raw() const { return m_so; }
	explicit operator bool() const { return m_so != nullptr; }
	struct wlr_output* output() const { return m_so->output; }
	void set_position(int x, int y) { wlr_scene_output_set_position(m_so, x, y); }
	bool commit() { return wlr_scene_output_commit(m_so, nullptr); }
	bool needs_frame() { return wlr_scene_output_needs_frame(m_so); }
	void send_frame_done(struct timespec* when) { wlr_scene_output_send_frame_done(m_so, when); }

private:
	struct wlr_scene_output* m_so;
};

class OutputLayout {
public:
	OutputLayout(Display& display) : m_layout(wlr_output_layout_create(display.raw())) {}
	~OutputLayout() { if (m_layout) wlr_output_layout_destroy(m_layout); }
	OutputLayout(const OutputLayout&) = delete;
	OutputLayout& operator=(const OutputLayout&) = delete;
	struct wlr_output_layout* raw() const { return m_layout; }
	explicit operator bool() const { return m_layout != nullptr; }
	struct wlr_output_layout_output* add_auto_output(struct wlr_output* o) {
		return wlr_output_layout_add_auto(m_layout, o);
	}
	struct wlr_output_layout_output* add_output(struct wlr_output* o, int x, int y) {
		return wlr_output_layout_add(m_layout, o, x, y);
	}
	struct wlr_box box() const {
		struct wlr_box b;
		wlr_output_layout_get_box(m_layout, nullptr, &b);
		return b;
	}

private:
	struct wlr_output_layout* m_layout;
};

}
