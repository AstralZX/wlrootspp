// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <functional>

#include "wlr/wlr.hpp"
#include "wlr/display.hpp"
#include "wlr/signals.hpp"
#include "wlr/xdg_shell.hpp"

namespace wlr {

class XdgToplevelDecoration {
public:
	enum Mode { None = 0, ClientSide = 1, ServerSide = 2 };

	XdgToplevelDecoration(struct wlr_xdg_toplevel_decoration_v1* d) : m_dec(d) {}
	struct wlr_xdg_toplevel_decoration_v1* raw() const { return m_dec; }
	explicit operator bool() const { return m_dec != nullptr; }

	Mode current_mode() const { return static_cast<Mode>(m_dec->current.mode); }
	Mode requested_mode() const { return static_cast<Mode>(m_dec->requested_mode); }
	void set_mode(Mode m) {
		wlr_xdg_toplevel_decoration_v1_set_mode(m_dec,
			static_cast<enum wlr_xdg_toplevel_decoration_v1_mode>(m));
	}

	Toplevel toplevel() const { return Toplevel{m_dec->toplevel}; }

	void on_request_mode(std::function<void(void*)> cb) {
		m_request.connect(&m_dec->events.request_mode, std::move(cb));
	}
	void on_destroy(std::function<void(void*)> cb) {
		m_destroy.connect(&m_dec->events.destroy, std::move(cb));
	}

private:
	struct wlr_xdg_toplevel_decoration_v1* m_dec;
	SignalSink m_request;
	SignalSink m_destroy;
};

class XdgDecorationManager {
public:
	XdgDecorationManager(Display& d)
		: m_mgr(wlr_xdg_decoration_manager_v1_create(d.raw())) {}
	struct wlr_xdg_decoration_manager_v1* raw() const { return m_mgr; }
	explicit operator bool() const { return m_mgr != nullptr; }

	void on_new_toplevel_decoration(std::function<void(void*)> cb) {
		m_new.connect(&m_mgr->events.new_toplevel_decoration, std::move(cb));
	}

private:
	struct wlr_xdg_decoration_manager_v1* m_mgr;
	SignalSink m_new;
};

}
