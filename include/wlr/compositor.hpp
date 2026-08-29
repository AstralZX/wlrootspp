// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include "wlr/display.hpp"
#include "wlr/signals.hpp"
#include "wlr/scene.hpp"
#include "wlr/node.hpp"
#include "wlr/output.hpp"

namespace wlr {

class Renderer {
public:
	Renderer(struct wlr_renderer* r) : m_renderer(r) {}
	struct wlr_renderer* raw() const { return m_renderer; }
	explicit operator bool() const { return m_renderer != nullptr; }
	bool init_wl_display(Display& d) { return wlr_renderer_init_wl_display(m_renderer, d.raw()); }
	void destroy() { if (m_renderer) { wlr_renderer_destroy(m_renderer); m_renderer = nullptr; } }
	~Renderer() { destroy(); }

private:
	struct wlr_renderer* m_renderer;
};

class Allocator {
public:
	Allocator(struct wlr_allocator* a) : m_allocator(a) {}
	struct wlr_allocator* raw() const { return m_allocator; }
	explicit operator bool() const { return m_allocator != nullptr; }
	void destroy() { if (m_allocator) { wlr_allocator_destroy(m_allocator); m_allocator = nullptr; } }
	~Allocator() { destroy(); }

private:
	struct wlr_allocator* m_allocator;
};

class DataDeviceManager {
public:
	DataDeviceManager(struct wlr_data_device_manager* m) : m_manager(m) {}
	struct wlr_data_device_manager* raw() const { return m_manager; }

private:
	struct wlr_data_device_manager* m_manager;
};

class Compositor {
public:
	Compositor(Display& display, Renderer& renderer)
		: m_compositor(wlr_compositor_create(display.raw(), 6, renderer.raw())) {}
	struct wlr_compositor* raw() const { return m_compositor; }
	explicit operator bool() const { return m_compositor != nullptr; }

private:
	struct wlr_compositor* m_compositor;
};

}
