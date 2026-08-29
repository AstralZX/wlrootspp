// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <functional>

#include "wlr/display.hpp"
#include "wlr/signals.hpp"
#include "wlr/compositor.hpp"

namespace wlr {

class Backend {
public:
	explicit Backend(Display& display, bool headless = false) {
		m_backend = wlr_backend_autocreate(display.event_loop(), nullptr);
	}

	~Backend() {
		m_newOutput.disconnect();
		m_newInput.disconnect();
		if (m_backend)
			wlr_backend_destroy(m_backend);
	}

	Backend(const Backend&) = delete;
	Backend& operator=(const Backend&) = delete;

	struct wlr_backend* raw() const { return m_backend; }
	explicit operator bool() const { return m_backend != nullptr; }
	bool start() { return wlr_backend_start(m_backend); }

	// cb receives struct wlr_output*
	void on_new_output(std::function<void(void*)> cb) {
		m_newOutput.connect(&m_backend->events.new_output, std::move(cb));
	}
	// cb receives struct wlr_input_device*
	void on_new_input(std::function<void(void*)> cb) {
		m_newInput.connect(&m_backend->events.new_input, std::move(cb));
	}

private:
	struct wlr_backend* m_backend;
	SignalSink m_newOutput;
	SignalSink m_newInput;
};

}
