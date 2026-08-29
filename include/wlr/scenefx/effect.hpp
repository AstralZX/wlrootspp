// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <cstdint>
#include <type_traits>

#include "wlr/scenefx/anim.hpp"
#include "wlr/scene.hpp"
#include "wlr/node.hpp"

namespace wlr::fx {

class Effect {
public:
	Animated<>& position_x() { return m_x; }
	Animated<>& position_y() { return m_y; }
	Animated<>& scale()      { return m_scale; }
	Animated<>& opacity()    { return m_opacity; }

	void on_frame(std::uint64_t time) {
		m_x.on_frame(time);
		m_y.on_frame(time);
		m_scale.on_frame(time);
		m_opacity.on_frame(time);
	}

	void apply_to(Node& node) {
		node.set_position((int)m_x.value(), (int)m_y.value());
		if (node.type() != WLR_SCENE_NODE_BUFFER)
			return;
		Buffer b = node.as<Buffer>();
		b.set_opacity(m_opacity.value());
		const float s = m_scale.value();
		const int dw = b.raw()->dst_width;
		const int dh = b.raw()->dst_height;
		if (s != 1.0f && dw > 0 && dh > 0)
			b.set_dest_size((int)(s * dw), (int)(s * dh));
	}

	bool animating() const {
		return m_x.animating() || m_y.animating() ||
		       m_scale.animating() || m_opacity.animating();
	}

private:
	Animated<> m_x;
	Animated<> m_y;
	Animated<> m_scale{1.0f};
	Animated<> m_opacity{1.0f};
};

}
