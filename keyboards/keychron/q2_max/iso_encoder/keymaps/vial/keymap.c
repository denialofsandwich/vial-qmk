/* Copyright 2024 ~ 2026 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include "macros/main.h"
#ifdef RGB_MATRIX_ENABLE
#    include "lpm.h"
#    include "keychron_rgb_type.h"
#endif

enum layers {
    MAC_BASE,
    WIN_BASE,
    MAC_FN1,
    WIN_FN1,
    FN2,
};

// Base these in Keychron's QK_KB custom range (NEW_SAFE_RANGE), not QK_USER/SAFE_RANGE,
// so Vial's customKeycodes[] (indexed from QK_KB_0) can give them friendly labels.
// The order here must match the appended entries in vial.json.
enum custom_keycodes {
    M0 = NEW_SAFE_RANGE,
    RCTRL_LOOP,
    REPEAT_LOOP,
    DREC,        // FN2+Tab: record/stop toggle (single tap = no delay, double tap = real delays)
    MSLOT_1,     // FN2+Q : macro slot 1
    MSLOT_2,     // FN2+W : macro slot 2
    MSLOT_3,     // FN2+E : macro slot 3
    MSLOT_4,     // FN2+R : macro slot 4
    MSLOT_5,     // FN2+T : macro slot 5
};
_Static_assert(MSLOT_5 <= 0x7E1F, "custom keycodes overflow the QK_KB range (0x7E1F)");

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_iso_68(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_MUTE,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,  KC_GRV,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCMMD, KC_LOPTN, KC_LCTL,                                KC_SPC,                                 KC_ROPTN, MO(MAC_FN1),  MO(FN2),  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_BASE] = LAYOUT_iso_68(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_MUTE,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  MO(WIN_FN1),  MO(FN2),  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN1] = LAYOUT_iso_68(
        KC_NUBS,  KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, UG_VALD,  UG_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            UG_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  M0,       _______,  _______,                      _______,
        UG_TOGG,  UG_NEXT,  UG_VALU,  UG_HUEU,  UG_SATU,  UG_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
        _______,  _______,  UG_PREV,  UG_VALD,  UG_HUED,  UG_SATD,  UG_SPDD,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_FN1] = LAYOUT_iso_68(
        KC_GRV,   KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  UG_VALD,  UG_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT, KC_MUTE,  KC_VOLD,   KC_VOLU,  KC_PSCR,            UG_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        UG_TOGG,  UG_NEXT,  UG_VALU,  UG_HUEU,  UG_SATU,  UG_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
        _______,  _______,  UG_PREV,  UG_VALD,  UG_HUED,  UG_SATD,  UG_SPDD,  _______,  _______,  _______,  _______,  KC_INT2,            _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

    [FN2] = LAYOUT_iso_68(
        KC_TILD,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,            _______,
        DREC,     MSLOT_1,  MSLOT_2,  MSLOT_3,  MSLOT_4,  MSLOT_5,  _______,  _______,  _______,  _______,  _______,  _______,  KC_BTN1,                      _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, REPEAT_LOOP,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        RCTRL_LOOP, _______,  _______,                              _______,                                _______,  _______,  _______,  _______,  _______,  _______)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [WIN_BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [MAC_FN1]  = { ENCODER_CCW_CW(UG_VALD, UG_VALU) },
    [WIN_FN1]  = { ENCODER_CCW_CW(UG_VALD, UG_VALU) },
    [FN2]      = { ENCODER_CCW_CW(_______, _______) }
};
#endif // ENCODER_MAP_ENABLE

static bool rctrl_loop_active  = false;
static bool repeat_loop_active = false;

// ---------------------------------------------------------------------------
// Custom multi-slot live macro recorder
//
// FN2+Tab     : arm recording (single tap = no delay, double tap = real delays)
// FN2+Q..T    : while armed -> start recording into that slot
//               while idle  -> play that slot
//               while playing -> stop (interrupt)
// FN2+Tab     : while recording -> stop
// FN2+Enter   : loop the last played slot (or, if none, the legacy "repeat last key")
//
// Storage is RAM-only (lost on power-off). Each event stores the keycode, the
// delay since the previous event, and whether it was a press or release.
// ---------------------------------------------------------------------------
typedef struct {
    uint16_t keycode;
    uint16_t delay_ms;  // time since previous recorded event
    bool     pressed;
} macro_event_t;

typedef enum { ST_IDLE, ST_ARMED, ST_RECORDING } macro_state_t;

static macro_event_t macro_store[MACRO_SLOT_COUNT][MACRO_MAX_EVENTS];
static uint16_t       macro_len[MACRO_SLOT_COUNT];       // events recorded per slot
static bool           macro_realdelay[MACRO_SLOT_COUNT]; // playback honors real delays?
static uint16_t       macro_trail[MACRO_SLOT_COUNT];     // last-key -> stop gap (loop spacing)

static macro_state_t  macro_state     = ST_IDLE;
static bool           armed_realdelay = false;           // mode chosen at arm time
static uint8_t        active_slot     = 0;
static uint32_t       drec_tap_timer  = 0;               // double-tap detection
static uint32_t       last_event_time = 0;               // for inter-event delta

static bool           playing         = false;           // non-blocking playback active
static bool           play_loop       = false;           // restart when finished
static uint8_t        play_slot_idx   = 0;
static uint16_t       play_pos        = 0;                // index into macro_store[slot]
static uint32_t       play_timer      = 0;                // when current step's wait started
static uint16_t       play_wait       = 0;                // ms to wait before processing play_pos
static int8_t         last_macro_slot = -1;              // last slot played (for FN2+D loop)

static inline int8_t slot_index(uint16_t keycode) {
    if (keycode >= MSLOT_1 && keycode <= MSLOT_5) return (int8_t)(keycode - MSLOT_1);
    return -1;
}

static uint16_t clamp_u16(uint32_t v) { return v > 0xFFFF ? 0xFFFF : (uint16_t)v; }

static bool is_recordable(uint16_t keycode, keyrecord_t *record) {
    if (record->event.type != KEY_EVENT) return false;   // skip encoder events
    if (keycode == DREC || slot_index(keycode) >= 0) return false;
    switch (keycode) {
        case M0:
        case RCTRL_LOOP:
        case REPEAT_LOOP:
            return false;
    }
    // don't record layer-switching keys (e.g. MO(FN2)) — they replay nonsensically
    if (IS_QK_MOMENTARY(keycode) || IS_QK_LAYER_TAP(keycode) || IS_QK_TO(keycode) ||
        IS_QK_TOGGLE_LAYER(keycode) || IS_QK_DEF_LAYER(keycode) ||
        IS_QK_LAYER_TAP_TOGGLE(keycode) || IS_QK_ONE_SHOT_LAYER(keycode) ||
        IS_QK_LAYER_MOD(keycode)) {
        return false;
    }
    return true;
}

static void stop_recording(void) {
    if (macro_state != ST_RECORDING) return;
    uint8_t s = active_slot;
    macro_trail[s] = (macro_len[s] == 0) ? 0 : clamp_u16(timer_elapsed32(last_event_time));

    // Append releases for any keys still held at stop, so playback (and looping)
    // never leaves a key stuck down.
    uint16_t held[MACRO_MAX_EVENTS];
    uint8_t  nheld = 0;
    for (uint16_t i = 0; i < macro_len[s]; i++) {
        uint16_t kc = macro_store[s][i].keycode;
        if (macro_store[s][i].pressed) {
            bool found = false;
            for (uint8_t h = 0; h < nheld; h++) if (held[h] == kc) { found = true; break; }
            if (!found && nheld < MACRO_MAX_EVENTS) held[nheld++] = kc;
        } else {
            for (uint8_t h = 0; h < nheld; h++) if (held[h] == kc) { held[h] = held[--nheld]; break; }
        }
    }
    for (uint8_t h = 0; h < nheld && macro_len[s] < MACRO_MAX_EVENTS; h++) {
        macro_store[s][macro_len[s]].keycode  = held[h];
        macro_store[s][macro_len[s]].delay_ms = 0;
        macro_store[s][macro_len[s]].pressed  = false;
        macro_len[s]++;
    }
    macro_state = ST_IDLE;
}

static void record_event(uint16_t keycode, bool pressed) {
    uint16_t *len = &macro_len[active_slot];
    if (*len >= MACRO_MAX_EVENTS) {  // buffer full -> auto-stop
        stop_recording();
        return;
    }
    uint16_t delta = (*len == 0) ? 0 : clamp_u16(timer_elapsed32(last_event_time));
    macro_store[active_slot][*len].keycode  = keycode;
    macro_store[active_slot][*len].delay_ms = delta;
    macro_store[active_slot][*len].pressed  = pressed;
    (*len)++;
    last_event_time = timer_read32();
}

static void start_recording(uint8_t slot) {
    active_slot           = slot;
    macro_len[slot]       = 0;
    macro_realdelay[slot] = armed_realdelay;
    macro_trail[slot]     = 0;
    last_event_time       = timer_read32();
    macro_state           = ST_RECORDING;
}

static void start_playback(uint8_t slot, bool loop) {
    if (macro_len[slot] == 0) return;
    playing       = true;
    play_loop     = loop;
    play_slot_idx = slot;
    play_pos      = 0;
    play_wait     = 0;
    play_timer    = timer_read32();
}

static void stop_playback(void) {
    playing   = false;
    play_loop = false;
    clear_keyboard();  // release anything the macro left held
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int8_t slot = slot_index(keycode);

    if (keycode == DREC) {
        if (record->event.pressed) {
            if (playing) {
                stop_playback();
                repeat_loop_active = false;
            }
            if (macro_state == ST_RECORDING) {
                stop_recording();
            } else if (macro_state == ST_ARMED && timer_elapsed32(drec_tap_timer) < DREC_TAP_TERM) {
                armed_realdelay = true;            // double tap -> measure real delays
            } else {
                armed_realdelay = false;           // single tap -> no delay
                macro_state     = ST_ARMED;
            }
            drec_tap_timer = timer_read32();
        }
        return false;
    }

    if (slot >= 0) {
        if (record->event.pressed) {
            if (playing) {                         // press during playback -> interrupt
                stop_playback();
                repeat_loop_active = false;
            } else if (macro_state == ST_ARMED) {
                start_recording((uint8_t)slot);
            } else if (macro_state == ST_IDLE && macro_len[slot] > 0) {
                last_macro_slot = slot;
                start_playback((uint8_t)slot, false);
            }
            // ST_RECORDING: slot keys are swallowed (not recordable)
        }
        return false;
    }

    // capture live keystrokes while recording
    if (macro_state == ST_RECORDING && is_recordable(keycode, record)) {
        record_event(keycode, record->event.pressed);
        return true;  // also type the key live
    }

    switch (keycode) {
        case M0:
            if (record->event.pressed) {
                SEND_STRING(M0_SEQ);
            }
            return false;
        case RCTRL_LOOP:
            if (record->event.pressed) {
                rctrl_loop_active = !rctrl_loop_active;
            }
            return false;
        case REPEAT_LOOP:
            if (record->event.pressed) {
                repeat_loop_active = !repeat_loop_active;
                if (repeat_loop_active) {
                    if (last_macro_slot >= 0 && macro_len[last_macro_slot] > 0) {
                        start_playback((uint8_t)last_macro_slot, true);  // loop the macro
                    }
                    // else: legacy "repeat last key" handled in matrix_scan_user
                } else {
                    stop_playback();
                }
            }
            return false;
    }
    return true;
}

// Keep our custom macro/toggle keycodes out of the Repeat Key memory, so the
// spam loop repeats the last "real" key pressed instead of the toggle itself.
bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    if (keycode == DREC || slot_index(keycode) >= 0) return false;
    switch (keycode) {
        case M0:
        case RCTRL_LOOP:
        case REPEAT_LOOP:
            return false;
    }
    return true;
}

#ifdef RGB_MATRIX_ENABLE
extern uint8_t per_key_rgb_type;

void matrix_scan_user(void) {
    // Non-blocking macro playback engine. Runs one event per elapsed wait so the
    // matrix keeps being scanned between events and playback stays interruptible.
    if (playing) {
        lpm_timer_reset();
        if (timer_elapsed32(play_timer) >= play_wait) {
            uint8_t s = play_slot_idx;
            if (play_pos >= macro_len[s]) {        // finished a pass
                if (play_loop) {
                    play_pos   = 0;
                    play_wait  = 0;
                    play_timer = timer_read32();
                } else {
                    stop_playback();
                }
            } else {
                macro_event_t *e = &macro_store[s][play_pos];
                if (e->pressed) register_code16(e->keycode);
                else            unregister_code16(e->keycode);
                play_pos++;
                if (play_pos >= macro_len[s]) {    // wait the trailing gap before looping
                    play_wait = play_loop ? (macro_realdelay[s] ? macro_trail[s] : MACRO_NODELAY_MS) : 0;
                } else {
                    play_wait = macro_realdelay[s] ? macro_store[s][play_pos].delay_ms : MACRO_NODELAY_MS;
                }
                play_timer = timer_read32();
            }
        }
    }

    static uint32_t repeat_timer = 0;
    // Legacy "repeat last key" only when the loop key isn't driving a macro.
    if (repeat_loop_active && last_macro_slot < 0) {
        lpm_timer_reset();
        if (timer_elapsed32(repeat_timer) >= 50) {
            repeat_timer = timer_read32();
            // Re-fire the last pressed key via the QMK Repeat Key feature.
            keyevent_t press = MAKE_KEYEVENT(0, 0, true);
            repeat_key_invoke(&press);
            keyevent_t release = MAKE_KEYEVENT(0, 0, false);
            repeat_key_invoke(&release);
        }
    } else {
        repeat_timer = timer_read32();
    }

    static uint32_t rctrl_timer = 0;
    if (rctrl_loop_active) {
        lpm_timer_reset();
        if (timer_elapsed32(rctrl_timer) >= 60000) {
            rctrl_timer = timer_read32();
            tap_code(KC_RCTL);
        }
    } else {
        rctrl_timer = timer_read32();
    }

    static bool     last_usb_state = false;
    static bool     initialized    = false;
    static uint32_t last_check     = 0;

    if (timer_elapsed32(last_check) < 200) return;
    last_check = timer_read32();

    // Use a full color effect using a wire and a low energy glow on battery
    bool usb_now = usb_power_connected();
    if (!initialized || usb_now != last_usb_state) {
        initialized    = true;
        last_usb_state = usb_now;
        if (usb_now) {
            per_key_rgb_type = PER_KEY_RGB_SOLID;
            rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_PER_KEY_RGB);
        } else {
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE);
        }
    }
}

// LED indices for the macro keys (matrix row 1: Tab, Q, W, E, R, T)
#define MACRO_TAB_LED 14
static const uint8_t macro_slot_led[MACRO_SLOT_COUNT] = {15, 16, 17, 18, 19};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (rctrl_loop_active && led_min <= 57 && 57 < led_max) {
        rgb_matrix_set_color(57, 0x00, 0xFF, 0x00);
    }
    if (repeat_loop_active && led_min <= 27 && 27 < led_max) {
        rgb_matrix_set_color(27, 0x00, 0xFF, 0x00);
    }
    // Armed: Tab key — yellow for no-delay, magenta for real-delay
    if (macro_state == ST_ARMED && led_min <= MACRO_TAB_LED && MACRO_TAB_LED < led_max) {
        if (armed_realdelay) rgb_matrix_set_color(MACRO_TAB_LED, 0xFF, 0x00, 0xFF);
        else                 rgb_matrix_set_color(MACRO_TAB_LED, 0xFF, 0xFF, 0x00);
    }
    // Recording: active slot key glows red
    if (macro_state == ST_RECORDING) {
        uint8_t idx = macro_slot_led[active_slot];
        if (led_min <= idx && idx < led_max) rgb_matrix_set_color(idx, 0xFF, 0x00, 0x00);
    }
    // Playing: slot key glows green
    if (playing) {
        uint8_t idx = macro_slot_led[play_slot_idx];
        if (led_min <= idx && idx < led_max) rgb_matrix_set_color(idx, 0x00, 0xFF, 0x00);
    }
    return false;
}
#endif
