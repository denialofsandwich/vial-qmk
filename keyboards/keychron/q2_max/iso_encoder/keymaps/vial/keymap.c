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
    MSLOT_6,     // FN2+Y : macro slot 6
    MSLOT_7,     // FN2+U : macro slot 7
    MSLOT_8,     // FN2+I : macro slot 8
    MSLOT_9,     // FN2+O : macro slot 9
    MSLOT_10,    // FN2+P : macro slot 10
};
_Static_assert(MSLOT_10 <= 0x7E1F, "custom keycodes overflow the QK_KB range (0x7E1F)");

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
        DREC,     MSLOT_1,  MSLOT_2,  MSLOT_3,  MSLOT_4,  MSLOT_5,  MSLOT_6,  MSLOT_7,  MSLOT_8,  MSLOT_9,  MSLOT_10, _______,  KC_BTN1,                      _______,
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
// FN2+Q..T    : while armed     -> start recording into that slot
//               while recording -> record a "call slot N" event (recursive macro)
//               while idle      -> play that slot
//               while playing   -> stop (interrupt)
// FN2+Tab     : while recording -> stop
// FN2+Enter   : loop the last played slot (or, if none, the legacy "repeat last key")
//
// Capacity: each slot holds MACRO_MAX_EVENTS events, but a recording is not
// limited to one slot. A macro event can be a key (EV_KEY) or a "call another
// slot" (EV_CALL); playback uses a stack so a slot can recurse into other slots
// and resume the caller when they return. Two ways calls appear:
//   * recursive call  — pressing a slot key while recording records EV_CALL.
//   * overflow chain  — when a slot fills mid-recording, an EV_CALL to the next
//                       sequential slot is appended and recording continues there
//                       (overwriting it). This lets one long macro span slots.
// LED feedback (see rgb_matrix_indicators_advanced_user): the slot currently
// executing is green, every caller still on the stack is yellow.
//
// Storage is RAM-only (lost on power-off). Each event stores the keycode (or
// target slot for EV_CALL), the delay since the previous event, and whether it
// was a press or release.
// ---------------------------------------------------------------------------
typedef enum { EV_KEY, EV_CALL } macro_evtype_t;

typedef struct {
    uint16_t keycode;   // EV_KEY: keycode to (un)register; EV_CALL: target slot index
    uint16_t delay_ms;  // time since previous recorded event
    uint8_t  type;      // EV_KEY or EV_CALL
    bool     pressed;
} macro_event_t;

typedef enum { ST_IDLE, ST_ARMED, ST_RECORDING } macro_state_t;

static macro_event_t macro_store[MACRO_SLOT_COUNT][MACRO_MAX_EVENTS];
static uint16_t       macro_len[MACRO_SLOT_COUNT];       // events recorded per slot
static bool           macro_realdelay[MACRO_SLOT_COUNT]; // playback honors real delays?
static uint16_t       macro_trail[MACRO_SLOT_COUNT];     // last-key -> stop gap (loop spacing)

static macro_state_t  macro_state     = ST_IDLE;
static bool           armed_realdelay = false;           // mode chosen at arm time
static uint8_t        active_slot     = 0;               // slot currently being recorded into
static uint8_t        rec_root_slot   = 0;               // slot the recording started in
static uint32_t       drec_tap_timer  = 0;               // double-tap detection
static uint32_t       last_event_time = 0;               // for inter-event delta

// Non-blocking playback uses a call stack so a slot can recurse into other slots
// (explicit recursive calls and overflow chaining both ride on EV_CALL events).
typedef struct { uint8_t slot; uint16_t pos; } play_frame_t;
#define MACRO_PLAY_STACK_DEPTH 16                        // caps recursion / cyclic calls

static play_frame_t   play_stack[MACRO_PLAY_STACK_DEPTH];
static uint8_t        play_depth      = 0;               // 0 == idle (replaces old `playing`)
static uint8_t        play_root       = 0;               // slot to restart from when looping
static uint8_t        play_last_key_slot = 0;            // slot of last real key (loop trail gap)
static bool           play_loop       = false;           // restart when finished
static uint32_t       play_timer      = 0;               // when current step's wait started
static uint16_t       play_wait       = 0;                // ms to wait before processing top frame
static int8_t         last_macro_slot = -1;              // last slot played (for FN2+Enter loop)

static inline int8_t slot_index(uint16_t keycode) {
    if (keycode >= MSLOT_1 && keycode <= MSLOT_10) return (int8_t)(keycode - MSLOT_1);
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
    // never leaves a key stuck down. Scan the whole recorded chain (rec_root_slot
    // .. active_slot, which is sequential) so a key pressed before an overflow
    // boundary and never released still gets a release at the very end.
    uint16_t held[MACRO_MAX_EVENTS];
    uint8_t  nheld = 0;
    for (uint8_t cs = rec_root_slot; cs <= s; cs++) {
        for (uint16_t i = 0; i < macro_len[cs]; i++) {
            if (macro_store[cs][i].type == EV_CALL) continue;
            uint16_t kc = macro_store[cs][i].keycode;
            if (macro_store[cs][i].pressed) {
                bool found = false;
                for (uint8_t h = 0; h < nheld; h++) if (held[h] == kc) { found = true; break; }
                if (!found && nheld < MACRO_MAX_EVENTS) held[nheld++] = kc;
            } else {
                for (uint8_t h = 0; h < nheld; h++) if (held[h] == kc) { held[h] = held[--nheld]; break; }
            }
        }
    }
    for (uint8_t h = 0; h < nheld && macro_len[s] < MACRO_MAX_EVENTS; h++) {
        macro_store[s][macro_len[s]].keycode  = held[h];
        macro_store[s][macro_len[s]].delay_ms = 0;
        macro_store[s][macro_len[s]].type     = EV_KEY;
        macro_store[s][macro_len[s]].pressed  = false;
        macro_len[s]++;
    }
    macro_state = ST_IDLE;
}

// Reserve the final buffer entry of every slot for a chain call: when the active
// slot is about to fill, append an EV_CALL to the next sequential slot and continue
// recording there (overwriting it). Returns false only when there is genuinely no
// slot left to chain into, so the caller should stop recording.
static bool ensure_record_room(void) {
    uint16_t *len = &macro_len[active_slot];
    if (*len < MACRO_MAX_EVENTS - 1) return true;       // room for one more event
    if (active_slot + 1 >= MACRO_SLOT_COUNT) return false;
    uint8_t next = active_slot + 1;
    macro_store[active_slot][*len].keycode  = next;     // chain call at the reserved entry
    macro_store[active_slot][*len].delay_ms = 0;
    macro_store[active_slot][*len].type     = EV_CALL;
    macro_store[active_slot][*len].pressed  = false;
    (*len)++;
    active_slot           = next;
    macro_len[next]       = 0;
    macro_realdelay[next] = armed_realdelay;
    macro_trail[next]     = 0;
    return true;
}

// Append an "execute slot `target`" event to the current recording. Used both for
// explicit recursive calls (pressing a slot key while recording) and indirectly via
// the overflow chain above.
static void record_call(uint8_t target) {
    if (!ensure_record_room()) { stop_recording(); return; }
    uint16_t *len  = &macro_len[active_slot];
    uint16_t delta = (*len == 0) ? 0 : clamp_u16(timer_elapsed32(last_event_time));
    macro_store[active_slot][*len].keycode  = target;
    macro_store[active_slot][*len].delay_ms = delta;
    macro_store[active_slot][*len].type     = EV_CALL;
    macro_store[active_slot][*len].pressed  = false;
    (*len)++;
    last_event_time = timer_read32();
}

static void record_event(uint16_t keycode, bool pressed) {
    if (!ensure_record_room()) { stop_recording(); return; }
    uint16_t *len  = &macro_len[active_slot];
    uint16_t delta = (*len == 0) ? 0 : clamp_u16(timer_elapsed32(last_event_time));
    macro_store[active_slot][*len].keycode  = keycode;
    macro_store[active_slot][*len].delay_ms = delta;
    macro_store[active_slot][*len].type     = EV_KEY;
    macro_store[active_slot][*len].pressed  = pressed;
    (*len)++;
    last_event_time = timer_read32();
}

static void start_recording(uint8_t slot) {
    active_slot           = slot;
    rec_root_slot         = slot;
    macro_len[slot]       = 0;
    macro_realdelay[slot] = armed_realdelay;
    macro_trail[slot]     = 0;
    last_event_time       = timer_read32();
    macro_state           = ST_RECORDING;
}

static void start_playback(uint8_t slot, bool loop) {
    if (macro_len[slot] == 0) return;
    play_stack[0].slot = slot;
    play_stack[0].pos  = 0;
    play_depth         = 1;
    play_root          = slot;
    play_last_key_slot = slot;
    play_loop          = loop;
    play_wait          = 0;
    play_timer         = timer_read32();
}

static void stop_playback(void) {
    play_depth = 0;
    play_loop  = false;
    clear_keyboard();  // release anything the macro left held
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int8_t slot = slot_index(keycode);

    if (keycode == DREC) {
        if (record->event.pressed) {
            if (play_depth > 0) {
                stop_playback();
                repeat_loop_active = false;
            }
            if (macro_state == ST_RECORDING) {
                stop_recording();
            } else if (macro_state == ST_ARMED) {
                if (timer_elapsed32(drec_tap_timer) < DREC_TAP_TERM) {
                    armed_realdelay = true;        // quick double tap -> measure real delays
                } else {
                    macro_state = ST_IDLE;         // settled press while armed -> abort
                }
            } else {
                armed_realdelay = false;           // single tap -> arm (no delay)
                macro_state     = ST_ARMED;
            }
            drec_tap_timer = timer_read32();
        }
        return false;
    }

    if (slot >= 0) {
        if (record->event.pressed) {
            if (play_depth > 0) {                  // press during playback -> interrupt
                stop_playback();
                repeat_loop_active = false;
            } else if (macro_state == ST_RECORDING) {
                record_call((uint8_t)slot);        // record a recursive call to that slot
            } else if (macro_state == ST_ARMED) {
                start_recording((uint8_t)slot);
            } else if (macro_state == ST_IDLE && macro_len[slot] > 0) {
                last_macro_slot = slot;
                start_playback((uint8_t)slot, false);
            }
        }
        return false;
    }

    // A normal key press becomes the new "last action", so FN2+Enter goes back to
    // repeating that key instead of staying stuck on the last played macro slot.
    // is_recordable() conveniently excludes layer keys (holding FN2 to reach Enter
    // won't reset this) and our own macro/toggle keycodes.
    if (record->event.pressed && is_recordable(keycode, record)) {
        last_macro_slot = -1;
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
    if (play_depth > 0) {
        lpm_timer_reset();
        if (timer_elapsed32(play_timer) >= play_wait) {
            play_frame_t *fr = &play_stack[play_depth - 1];
            uint8_t s = fr->slot;
            if (fr->pos >= macro_len[s]) {         // current frame finished -> pop
                play_depth--;
                if (play_depth == 0) {             // whole macro finished
                    if (play_loop) {
                        play_stack[0].slot = play_root;
                        play_stack[0].pos  = 0;
                        play_depth         = 1;
                        // trailing gap before looping, based on the last slot that typed
                        uint8_t ls = play_last_key_slot;
                        play_wait  = macro_realdelay[ls] ? macro_trail[ls] : MACRO_NODELAY_MS;
                    } else {
                        stop_playback();
                    }
                } else {                           // resumed the caller
                    play_wait = MACRO_NODELAY_MS;
                }
                play_timer = timer_read32();
            } else {
                macro_event_t *e = &macro_store[s][fr->pos];
                if (e->type == EV_CALL) {
                    uint8_t target = (uint8_t)e->keycode;
                    fr->pos++;                     // advance past the call in this frame
                    if (play_depth < MACRO_PLAY_STACK_DEPTH && macro_len[target] > 0) {
                        play_stack[play_depth].slot = target;
                        play_stack[play_depth].pos  = 0;
                        play_depth++;
                    }                              // else: depth cap / empty target -> skip
                    play_wait = MACRO_NODELAY_MS;
                } else {
                    if (e->pressed) register_code16(e->keycode);
                    else            unregister_code16(e->keycode);
                    play_last_key_slot = s;
                    fr->pos++;
                    play_wait = (fr->pos < macro_len[s])
                                ? (macro_realdelay[s] ? macro_store[s][fr->pos].delay_ms : MACRO_NODELAY_MS)
                                : MACRO_NODELAY_MS;
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

// LED indices for the macro keys (matrix row 1: Tab, Q, W, E, R, T, Y, U, I, O, P)
#define MACRO_TAB_LED 14
static const uint8_t macro_slot_led[MACRO_SLOT_COUNT] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

// General macro-status colors (one place to retune them).
#define MACRO_C_ACTIVE   0x00, 0xFF, 0x00   // green   — executing slot / active loop toggle
#define MACRO_C_PARENT   0xFF, 0xFF, 0x00   // yellow  — caller still on the play stack
#define MACRO_C_RECORD   0xFF, 0x00, 0x00   // red     — slot being recorded into
#define MACRO_C_ARMED_RD 0xFF, 0x00, 0xFF   // magenta — armed, real-delay mode
#define MACRO_C_ARMED_ND 0xFF, 0xFF, 0x00   // yellow  — armed, no-delay mode

// Paint a single LED only when it falls in the current render window. Pass NO_LED
// (or a disabled state via the ternary at the call site) to skip.
static void macro_led(uint8_t lo, uint8_t hi, uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
    if (idx != NO_LED && lo <= idx && idx < hi) rgb_matrix_set_color(idx, r, g, b);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Loop-style toggles: solid green while active.
    macro_led(led_min, led_max, rctrl_loop_active  ? 57 : NO_LED, MACRO_C_ACTIVE);
    macro_led(led_min, led_max, repeat_loop_active ? 27 : NO_LED, MACRO_C_ACTIVE);

    // Armed: Tab key — magenta for real-delay, yellow for no-delay.
    if (macro_state == ST_ARMED) {
        if (armed_realdelay) macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_RD);
        else                 macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_ND);
    }

    // Recording: the slot currently being written glows red.
    if (macro_state == ST_RECORDING) {
        macro_led(led_min, led_max, macro_slot_led[active_slot], MACRO_C_RECORD);
    }

    // Playing: the call stack — callers yellow, the executing (top) slot green.
    // Green is written last so it wins when a slot appears at multiple depths.
    if (play_depth > 0) {
        for (uint8_t i = 0; i + 1 < play_depth; i++) {
            macro_led(led_min, led_max, macro_slot_led[play_stack[i].slot], MACRO_C_PARENT);
        }
        macro_led(led_min, led_max, macro_slot_led[play_stack[play_depth - 1].slot], MACRO_C_ACTIVE);
    }
    return false;
}
#endif
