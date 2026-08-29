// SPDX-License-Identifier: AGPL-3.0-only
#include "wlr/node.hpp"
#include <string>
#include <wlr/version.h>

namespace wlr {

std::string version() {
	return std::to_string(wlr_version_get_major()) + "." +
	       std::to_string(wlr_version_get_minor()) + "." +
	       std::to_string(wlr_version_get_micro());
}

template <> Tree    Node::as<Tree>()   const { return Tree{wlr_scene_tree_from_node(m_node)}; }
template <> Rect    Node::as<Rect>()   const { return Rect{wlr_scene_rect_from_node(m_node)}; }
template <> Shadow  Node::as<Shadow>() const { return Shadow{wlr_scene_shadow_from_node(m_node)}; }
template <> Blur    Node::as<Blur>()   const { return Blur{wlr_scene_blur_from_node(m_node)}; }
template <> OptimizedBlur Node::as<OptimizedBlur>() const {
	return OptimizedBlur{wlr_scene_optimized_blur_from_node(m_node)};
}
template <> Buffer  Node::as<Buffer>() const { return Buffer{wlr_scene_buffer_from_node(m_node)}; }
template <> Surface Node::as<Surface>() const {
	return Surface{wlr_scene_surface_try_from_buffer(wlr_scene_buffer_from_node(m_node))};
}

}
