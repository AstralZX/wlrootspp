// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <pixman.h>
#include <string>

#include "wlr/wlr.hpp"

namespace wlr {

std::string version();

class Scene {
public:
	Scene() : m_scene(wlr_scene_create()) {}
	~Scene() { if (m_scene) wlr_scene_node_destroy(&m_scene->tree.node); }

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	struct wlr_scene* raw() const { return m_scene; }
	explicit operator bool() const { return m_scene != nullptr; }

	void set_blur_data(int num_passes, int radius, float noise,
		float brightness, float contrast, float saturation) {
		wlr_scene_set_blur_data(m_scene, num_passes, radius,
			noise, brightness, contrast, saturation);
	}
	void set_blur_num_passes(int num_passes) { wlr_scene_set_blur_num_passes(m_scene, num_passes); }
	void set_blur_radius(int radius) { wlr_scene_set_blur_radius(m_scene, radius); }
	void set_blur_noise(float noise) { wlr_scene_set_blur_noise(m_scene, noise); }
	void set_blur_brightness(float brightness) { wlr_scene_set_blur_brightness(m_scene, brightness); }
	void set_blur_contrast(float contrast) { wlr_scene_set_blur_contrast(m_scene, contrast); }
	void set_blur_saturation(float saturation) { wlr_scene_set_blur_saturation(m_scene, saturation); }
	struct wlr_scene_tree* root() { return &m_scene->tree; }

private:
	struct wlr_scene* m_scene;
};

struct Box {
	int x = 0, y = 0, width = 0, height = 0;
	pixman_box32 to_box() const { return {x, y, x + width, y + height}; }
	static Box from_box(pixman_box32 b) {
		return {b.x1, b.y1, b.x2 - b.x1, b.y2 - b.y1};
	}
};

struct Color {
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
	void apply(float out[4]) const {
		out[0] = r; out[1] = g; out[2] = b; out[3] = a;
	}
};

struct CornerRadii {
	uint16_t top_left = 0, top_right = 0, bottom_right = 0, bottom_left = 0;

	CornerRadii() = default;
	CornerRadii(uint16_t all) : top_left(all), top_right(all), bottom_right(all), bottom_left(all) {}
	CornerRadii(uint16_t tl, uint16_t tr, uint16_t br, uint16_t bl)
		: top_left(tl), top_right(tr), bottom_right(br), bottom_left(bl) {}

	fx_corner_radii raw() const {
		return fx_corner_radii{top_left, top_right, bottom_right, bottom_left};
	}
	static CornerRadii from_radii(fx_corner_radii r) {
		return {r.top_left, r.top_right, r.bottom_right, r.bottom_left};
	}
};

}
