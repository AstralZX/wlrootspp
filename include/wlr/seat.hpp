// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <functional>
#include <xkbcommon/xkbcommon.h>

#include "wlr/wlr.hpp"
#include "wlr/display.hpp"
#include "wlr/signals.hpp"
#include "wlr/output.hpp"
namespace wlr {

class Cursor {
public:
	Cursor() : m_cursor(wlr_cursor_create()) {}
	~Cursor() { if (m_cursor) wlr_cursor_destroy(m_cursor); }
	Cursor(const Cursor&) = delete;
	Cursor& operator=(const Cursor&) = delete;
	struct wlr_cursor* raw() const { return m_cursor; }
	explicit operator bool() const { return m_cursor != nullptr; }

	void attach_output_layout(struct wlr_output_layout* layout) {
		wlr_cursor_attach_output_layout(m_cursor, layout);
	}
	void attach_input_device(struct wlr_input_device* dev) {
		wlr_cursor_attach_input_device(m_cursor, dev);
	}
	void warp(double x, double y) { wlr_cursor_warp(m_cursor, nullptr, x, y); }

private:
	struct wlr_cursor* m_cursor;
};

class Keyboard {
public:
	Keyboard(struct wlr_keyboard* kb) : m_keyboard(kb) {}
	struct wlr_keyboard* raw() const { return m_keyboard; }
	explicit operator bool() const { return m_keyboard != nullptr; }

	void set_keymap(struct xkb_keymap* keymap) { wlr_keyboard_set_keymap(m_keyboard, keymap); }
	void set_default_keymap() {
		struct xkb_context* ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		if (!ctx) return;
		struct xkb_keymap* km = xkb_keymap_new_from_names(ctx, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
		xkb_context_unref(ctx);
		if (km) {
			wlr_keyboard_set_keymap(m_keyboard, km);
			xkb_keymap_unref(km);
		}
	}
	void on_key(std::function<void(void*)> cb) { m_keys.connect(&m_keyboard->events.key, std::move(cb)); }

private:
	struct wlr_keyboard* m_keyboard;
	SignalSink m_keys;
};

class Seat {
public:
	Seat(Display& display, const char* name)
		: m_seat(wlr_seat_create(display.raw(), name)) {}
	~Seat() { if (m_seat) wlr_seat_destroy(m_seat); }
	Seat(const Seat&) = delete;
	Seat& operator=(const Seat&) = delete;

	struct wlr_seat* raw() const { return m_seat; }
	explicit operator bool() const { return m_seat != nullptr; }

	void set_capabilities(uint32_t caps) { wlr_seat_set_capabilities(m_seat, caps); }
	void set_keyboard(Keyboard& kb) { wlr_seat_set_keyboard(m_seat, kb.raw()); }
	void keyboard_notify_key(uint32_t time, uint32_t keycode, enum wl_keyboard_key_state state) {
		wlr_seat_keyboard_notify_key(m_seat, time, keycode, state);
	}
	void pointer_notify_motion(uint32_t time, double sx, double sy) {
		wlr_seat_pointer_notify_motion(m_seat, time, sx, sy);
	}
	uint32_t pointer_notify_button(uint32_t time, uint32_t button, enum wl_pointer_button_state state) {
		return wlr_seat_pointer_notify_button(m_seat, time, button, state);
	}
	void pointer_notify_axis(uint32_t time, enum wl_pointer_axis axis, double value, int value_discrete, enum wl_pointer_axis_source source) {
		wlr_seat_pointer_notify_axis(m_seat, time, axis, value, value_discrete, source,
			WL_POINTER_AXIS_RELATIVE_DIRECTION_IDENTICAL);
	}
	void pointer_notify_frame() { wlr_seat_pointer_notify_frame(m_seat); }

private:
	struct wlr_seat* m_seat;
};

}
