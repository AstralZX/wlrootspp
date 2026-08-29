// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>

namespace wlr::fx {

enum class Easing {
	Linear,
	InQuad, OutQuad, InOutQuad,
	InCubic, OutCubic, InOutCubic,
	InElastic, OutElastic, InOutElastic,
};

inline float ease(Easing e, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	switch (e) {
	case Easing::Linear:      return t;
	case Easing::InQuad:      return t * t;
	case Easing::OutQuad:     return t * (2.0f - t);
	case Easing::InCubic:     return t * t * t;
	case Easing::OutCubic:    { float u = t - 1.0f; return u * u * u + 1.0f; }
	case Easing::InOutQuad:   return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
	case Easing::InOutCubic:  return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
	case Easing::InElastic: {
		const float c4 = (2.0f * M_PI) / 3.0f;
		return t <= 0.0f ? 0.0f : t >= 1.0f ? 1.0f
			: -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
	}
	case Easing::OutElastic: {
		const float c4 = (2.0f * M_PI) / 3.0f;
		return t <= 0.0f ? 0.0f : t >= 1.0f ? 1.0f
			: std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
	}
	case Easing::InOutElastic: {
		const float c5 = (2.0f * M_PI) / 4.5f;
		return t <= 0.0f ? 0.0f : t >= 1.0f ? 1.0f
			: t < 0.5f
			  ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f
			  : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
	}
	}
	return t;
}

template <typename Clock = std::uint64_t>
class Animated {
public:
	Animated() = default;
	Animated(float value) : m_value(value), m_target(value), m_equal(true) {}

	void set(float value) {
		if (value == m_target && !m_equal) { jump_to(value); return; }
		m_target = value;
		if (m_duration == 0) { jump_to(value); return; }
		m_startValue = m_value;
		m_startTime = now();
		m_elapsed = 0;
		m_equal = false;
	}

	void jump_to(float value) {
		m_value = value; m_target = value; m_equal = true; m_startTime = now();
	}

	void on_frame(Clock time) {
		if (m_equal) return;
		m_elapsed = time - m_startTime;
		if (m_elapsed >= m_duration) { m_value = m_target; m_equal = true; return; }
		const float t = ease(m_easing, static_cast<float>(m_elapsed) / static_cast<float>(m_duration));
		m_value = m_startValue + (m_target - m_startValue) * t;
	}

	void set_duration(Clock d) { m_duration = d > 0 ? d : 1; }
	void set_easing(Easing e) { m_easing = e; }
	void set_clock_source(std::function<Clock()> src) { m_clock = std::move(src); }

	float value() const { return m_value; }
	float target() const { return m_target; }
	bool animating() const { return !m_equal; }
	Clock remaining() const { return m_equal ? 0 : m_duration - m_elapsed; }

private:
	Clock now() { return m_clock ? m_clock() : Clock(0); }
	float m_value = 0.0f;
	float m_startValue = 0.0f;
	float m_target = 0.0f;
	Clock m_duration = 1;
	Clock m_startTime = 0;
	Clock m_elapsed = 0;
	bool m_equal = true;
	Easing m_easing = Easing::OutCubic;
	std::function<Clock()> m_clock;
};

}
