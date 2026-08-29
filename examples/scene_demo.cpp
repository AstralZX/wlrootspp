// SPDX-License-Identifier: AGPL-3.0-only
#include <cstdio>
#include <cstdint>

#include <wlr/scene.hpp>
#include <wlr/node.hpp>
#include <wlr/scenefx/anim.hpp>
#include <wlr/scenefx/effect.hpp>

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	wlr::Scene scene;
	if (!scene)
		return 1;

	wlr::Tree root{wlr_scene_tree_create(&scene.raw()->tree)};

	const float color[4] = {1.0f, 0.2f, 0.2f, 1.0f};
	wlr::Rect r{wlr_scene_rect_create(root.raw(), 200, 120, color)};
	r.set_position(40, 60);

	wlr::fx::Effect fx;
	fx.position_y().set_duration(500);
	fx.position_y().set_easing(wlr::fx::Easing::OutCubic);
	fx.opacity().set_duration(300);
	fx.opacity().set_easing(wlr::fx::Easing::InOutQuad);

	fx.position_y().set(240.0f);
	fx.opacity().set(0.3f);

	std::uint64_t t = 0;
	while (fx.animating()) {
		t += 16;
		fx.on_frame(t);
		fx.apply_to(r);
	}

	wlr::Box box = {10, 20, 300, 40};
	wlr::Box back = wlr::Box::from_box(box.to_box());
	std::printf("wlroots++ demo: x=%d y=%d box=%dx%d anim=%s version=%s\n",
		r.position().first, r.position().second,
		back.width, back.height,
		fx.animating() ? "yes" : "no",
		wlr::version().c_str());

	return 0;
}
