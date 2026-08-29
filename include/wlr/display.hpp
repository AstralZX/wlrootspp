// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <wayland-server-core.h>

#include "wlr/wlr.hpp"

namespace wlr {

class Display {
public:
	Display() : m_display(wl_display_create()) {}
	~Display() { if (m_display) wl_display_destroy(m_display); }

	Display(const Display&) = delete;
	Display& operator=(const Display&) = delete;
	Display(Display&&) = delete;
	Display& operator=(Display&&) = delete;

	struct wl_display* raw() const { return m_display; }
	explicit operator bool() const { return m_display != nullptr; }
	struct wl_event_loop* event_loop() const { return wl_display_get_event_loop(m_display); }

	void set_terminate() { wl_display_terminate(m_display); }
	void run() { wl_display_run(m_display); }
	bool init_shm() { return wl_display_init_shm(m_display); }
	const char* add_socket_auto() {
		const char* name = wl_display_add_socket_auto(m_display);
		return name;
	}
	void flush_clients() { wl_display_flush_clients(m_display); }

private:
	struct wl_display* m_display;
};

}
