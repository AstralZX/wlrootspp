// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <functional>
#include <utility>

#include <wayland-server-core.h>

namespace wlr {

class SignalSink {
public:
	SignalSink() = default;
	SignalSink(const SignalSink&) = delete;
	SignalSink& operator=(const SignalSink&) = delete;

	void connect(struct wl_signal* sig, std::function<void(void*)> fn) {
		if (m_listener && m_listener->attached)
			return;
		if (!m_listener)
			m_listener = new Listener;
		m_listener->attached = true;
		m_listener->fn = std::move(fn);
		m_listener->listener.notify = &SignalSink::trampoline;
		wl_signal_add(sig, &m_listener->listener);
	}

	void disconnect() {
		if (m_listener && m_listener->attached) {
			wl_list_remove(&m_listener->listener.link);
			m_listener->attached = false;
		}
	}

	~SignalSink() {
		disconnect();
		delete m_listener;
	}

private:
	struct Listener {
		struct wl_listener listener{};
		std::function<void(void*)> fn;
		bool attached = false;
	};

	static void trampoline(struct wl_listener* l, void* data) {
		auto* self = wl_container_of(l, static_cast<Listener*>(nullptr), listener);
		if (self->fn)
			self->fn(data);
	}

	Listener* m_listener = nullptr;
};

}
