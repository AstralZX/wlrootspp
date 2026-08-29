// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <wlr/util/box.h>

#include "wlr/wlr.hpp"

namespace wlr {

class Geometry {
public:
	Geometry() = default;
	Geometry(int x, int y, int w, int h) : m_box{x, y, w, h} {}
	explicit Geometry(const struct wlr_box& b) : m_box(b) {}

	struct wlr_box* raw() { return &m_box; }
	const struct wlr_box* raw() const { return &m_box; }

	int x() const { return m_box.x; }
	int y() const { return m_box.y; }
	int width() const { return m_box.width; }
	int height() const { return m_box.height; }

	void set(int x, int y, int w, int h) {
		m_box.x = x; m_box.y = y; m_box.width = w; m_box.height = h;
	}

	bool contains(double px, double py) const {
		return wlr_box_contains_point(&m_box, px, py);
	}
	bool intersects(const Geometry& o) const {
		struct wlr_box dest;
		return wlr_box_intersection(&dest, &m_box, &o.m_box);
	}

private:
	struct wlr_box m_box{};
};

}
