// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <string>

#include "wlr/wlr.hpp"
#include "wlr/display.hpp"
#include "wlr/signals.hpp"
#include "wlr/node.hpp"

namespace wlr {

class XdgSurface;

class Toplevel {
public:
	Toplevel(struct wlr_xdg_toplevel* t) : m_toplevel(t) {}
	struct wlr_xdg_toplevel* raw() const { return m_toplevel; }
	explicit operator bool() const { return m_toplevel != nullptr; }

	XdgSurface base() const;
	struct wlr_surface* surface() const;

	std::string title() const { return m_toplevel->title ? m_toplevel->title : ""; }
	std::string app_id() const { return m_toplevel->app_id ? m_toplevel->app_id : ""; }
	int width() const { return m_toplevel->current.width; }
	int height() const { return m_toplevel->current.height; }
	bool maximized() const { return m_toplevel->current.maximized; }
	bool activated() const { return m_toplevel->current.activated; }
	struct wlr_xdg_toplevel* parent() const { return m_toplevel->parent; }

	uint32_t set_size(int w, int h) { return wlr_xdg_toplevel_set_size(m_toplevel, w, h); }
	uint32_t set_activated(bool activated) { return wlr_xdg_toplevel_set_activated(m_toplevel, activated); }
	uint32_t set_maximized(bool maximized) { return wlr_xdg_toplevel_set_maximized(m_toplevel, maximized); }

	void on_request_move(std::function<void(void*)> cb) {
		m_requestMove.connect(&m_toplevel->events.request_move, std::move(cb));
	}
	void on_request_resize(std::function<void(void*)> cb) {
		m_requestResize.connect(&m_toplevel->events.request_resize, std::move(cb));
	}
	void on_request_maximize(std::function<void(void*)> cb) {
		m_requestMaximize.connect(&m_toplevel->events.request_maximize, std::move(cb));
	}
	void on_request_minimize(std::function<void(void*)> cb) {
		m_requestMinimize.connect(&m_toplevel->events.request_minimize, std::move(cb));
	}
	void on_request_fullscreen(std::function<void(void*)> cb) {
		m_requestFullscreen.connect(&m_toplevel->events.request_fullscreen, std::move(cb));
	}
	void on_set_title(std::function<void(void*)> cb) {
		m_setTitle.connect(&m_toplevel->events.set_title, std::move(cb));
	}
	void on_set_app_id(std::function<void(void*)> cb) {
		m_setAppId.connect(&m_toplevel->events.set_app_id, std::move(cb));
	}

public:
	struct wlr_xdg_toplevel* m_toplevel;
	SignalSink m_requestMove;
	SignalSink m_requestResize;
	SignalSink m_requestMaximize;
	SignalSink m_requestMinimize;
	SignalSink m_requestFullscreen;
	SignalSink m_setTitle;
	SignalSink m_setAppId;
};

class XdgSurface {
public:
	XdgSurface(struct wlr_xdg_surface* s) : m_surface(s) {}
	struct wlr_xdg_surface* raw() const { return m_surface; }
	explicit operator bool() const { return m_surface != nullptr; }
	bool toplevel_role() const { return m_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL && m_surface->toplevel; }
	bool popup_role() const { return m_surface->role == WLR_XDG_SURFACE_ROLE_POPUP && m_surface->popup; }
	struct wlr_surface* surface() const { return m_surface->surface; }
	uint32_t geometry_width() const { return m_surface->geometry.width; }
	uint32_t geometry_height() const { return m_surface->geometry.height; }
	Toplevel toplevel() const { return Toplevel{m_surface->toplevel}; }
	struct wlr_box geometry() const {
		return m_surface->geometry;
	}

	void on_map(std::function<void(void*)> cb) {
		m_map.connect(&m_surface->surface->events.map, std::move(cb));
	}
	void on_unmap(std::function<void(void*)> cb) {
		m_unmap.connect(&m_surface->surface->events.unmap, std::move(cb));
	}
	void on_destroy(std::function<void(void*)> cb) {
		m_destroy.connect(&m_surface->events.destroy, std::move(cb));
	}

public:
	struct wlr_xdg_surface* m_surface;
	SignalSink m_map;
	SignalSink m_unmap;
	SignalSink m_destroy;
};

inline XdgSurface Toplevel::base() const { return XdgSurface{m_toplevel->base}; }
inline struct wlr_surface* Toplevel::surface() const { return m_toplevel->base->surface; }

class XdgShell {
public:
	explicit XdgShell(Display& display, uint32_t version = 5)
		: m_shell(wlr_xdg_shell_create(display.raw(), version)) {}
	struct wlr_xdg_shell* raw() const { return m_shell; }
	explicit operator bool() const { return m_shell != nullptr; }

	void on_new_toplevel(std::function<void(void*)> cb) {
		m_newToplevel.connect(&m_shell->events.new_toplevel, std::move(cb));
	}
	void on_new_surface(std::function<void(void*)> cb) {
		m_newSurface.connect(&m_shell->events.new_surface, std::move(cb));
	}

private:
	struct wlr_xdg_shell* m_shell;
	SignalSink m_newToplevel;
	SignalSink m_newSurface;
};

}
