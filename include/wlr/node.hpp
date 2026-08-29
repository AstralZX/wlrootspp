// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <utility>

#include "wlr/wlr.hpp"
#include "wlr/scene.hpp"

namespace wlr {

class Node {
public:
	Node(struct wlr_scene_node* node) : m_node(node) {}
	struct wlr_scene_node* raw() const { return m_node; }
	explicit operator bool() const { return m_node != nullptr; }

	void set_position(int x, int y) { wlr_scene_node_set_position(m_node, x, y); }
	std::pair<int, int> position() const { return {m_node->x, m_node->y}; }

	void set_enabled(bool enabled) { wlr_scene_node_set_enabled(m_node, enabled); }
	bool enabled() const { return m_node->enabled; }

	void raise_to_top() { wlr_scene_node_raise_to_top(m_node); }
	void lower_to_bottom() { wlr_scene_node_lower_to_bottom(m_node); }
	void place_above(Node& sibling) { wlr_scene_node_place_above(m_node, sibling.raw()); }
	void place_below(Node& sibling) { wlr_scene_node_place_below(m_node, sibling.raw()); }

	enum wlr_scene_node_type type() const { return m_node->type; }
	struct wlr_scene_tree* parent() const { return m_node->parent; }
	void*& data() { return m_node->data; }
	void* data() const { return m_node->data; }

	template <typename T> T as() const;

protected:
	struct wlr_scene_node* m_node;
};

class Tree : public Node {
public:
	Tree(struct wlr_scene_tree* tree) : Node(&tree->node), m_tree(tree) {}
	struct wlr_scene_tree* raw() const { return m_tree; }
	void reparent(Node& n) { wlr_scene_node_reparent(n.raw(), m_tree); }

private:
	struct wlr_scene_tree* m_tree;
};

class Rect : public Node {
public:
	Rect(struct wlr_scene_rect* rect) : Node(&rect->node), m_rect(rect) {}
	struct wlr_scene_rect* raw() const { return m_rect; }

	void set_size(int w, int h) { wlr_scene_rect_set_size(m_rect, w, h); }
	void set_color(const Color& c) { c.apply(m_rect->color); }
	void set_corner_radius(int radius) { wlr_scene_rect_set_corner_radius(m_rect, radius); }
	void set_corner_radii(const CornerRadii& r) { wlr_scene_rect_set_corner_radii(m_rect, r.raw()); }

private:
	struct wlr_scene_rect* m_rect;
};

class Shadow : public Node {
public:
	Shadow(struct wlr_scene_shadow* shadow) : Node(&shadow->node), m_shadow(shadow) {}
	struct wlr_scene_shadow* raw() const { return m_shadow; }

	void set_size(int w, int h) { wlr_scene_shadow_set_size(m_shadow, w, h); }
	void set_corner_radius(int radius) { wlr_scene_shadow_set_corner_radius(m_shadow, radius); }
	void set_blur_sigma(float sigma) { wlr_scene_shadow_set_blur_sigma(m_shadow, sigma); }
	void set_color(const Color& c) { c.apply(m_shadow->color); }

private:
	struct wlr_scene_shadow* m_shadow;
};

class Blur : public Node {
public:
	Blur(struct wlr_scene_blur* blur) : Node(&blur->node), m_blur(blur) {}
	struct wlr_scene_blur* raw() const { return m_blur; }

	void set_size(int w, int h) { wlr_scene_blur_set_size(m_blur, w, h); }
	void set_corner_radius(int radius) { wlr_scene_blur_set_corner_radius(m_blur, radius); }
	void set_corner_radii(const CornerRadii& r) { wlr_scene_blur_set_corner_radii(m_blur, r.raw()); }
	void set_alpha(float alpha) { wlr_scene_blur_set_alpha(m_blur, alpha); }
	void set_strength(float strength) { wlr_scene_blur_set_strength(m_blur, strength); }
	void set_should_only_blur_bottom_layer(bool v) { wlr_scene_blur_set_should_only_blur_bottom_layer(m_blur, v); }

private:
	struct wlr_scene_blur* m_blur;
};

class OptimizedBlur : public Node {
public:
	OptimizedBlur(struct wlr_scene_optimized_blur* b) : Node(&b->node), m_blur(b) {}
	struct wlr_scene_optimized_blur* raw() const { return m_blur; }

	void set_size(int w, int h) { wlr_scene_optimized_blur_set_size(m_blur, w, h); }
	void mark_dirty() { wlr_scene_optimized_blur_mark_dirty(m_blur); }

private:
	struct wlr_scene_optimized_blur* m_blur;
};

class Buffer : public Node {
public:
	Buffer(struct wlr_scene_buffer* buf) : Node(&buf->node), m_buffer(buf) {}
	struct wlr_scene_buffer* raw() const { return m_buffer; }

	void set_buffer(struct wlr_buffer* b) { wlr_scene_buffer_set_buffer(m_buffer, b); }
	void set_opacity(float o) { wlr_scene_buffer_set_opacity(m_buffer, o); }
	float opacity() const { return m_buffer->opacity; }
	void set_dest_size(int w, int h) { wlr_scene_buffer_set_dest_size(m_buffer, w, h); }
	void set_corner_radius(int radius) { wlr_scene_buffer_set_corner_radius(m_buffer, radius); }
	void set_corner_radii(const CornerRadii& r) { wlr_scene_buffer_set_corner_radii(m_buffer, r.raw()); }

private:
	struct wlr_scene_buffer* m_buffer;
};

class Surface : public Buffer {
public:
	Surface(struct wlr_scene_surface* s) : Buffer(s ? s->buffer : nullptr), m_surface(s) {}
	struct wlr_scene_surface* raw() const { return m_surface; }

private:
	struct wlr_scene_surface* m_surface;
};

}
