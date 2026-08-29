// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <string>

#include "wlr/wlr.hpp"
#include "wlr/color.hpp"
#include "wlr/node.hpp"

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

	Tree add_tree() { return Tree{wlr_scene_tree_create(&m_scene->tree)}; }
	Rect add_rect(int w, int h, const Color& c) {
		float col[4]; c.apply(col);
		return Rect{wlr_scene_rect_create(&m_scene->tree, w, h, col)};
	}
	Buffer add_buffer() { return Buffer{wlr_scene_buffer_create(&m_scene->tree, nullptr)}; }
	Surface add_surface(struct wlr_surface* s) { return Surface{wlr_scene_surface_create(&m_scene->tree, s)}; }

private:
	struct wlr_scene* m_scene;
};

}
