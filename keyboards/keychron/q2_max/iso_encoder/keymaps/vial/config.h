/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// Custom multi-slot live macro recorder (see keymap.c)
#define MACRO_SLOT_COUNT 10    // slots per bank (RAM and EEPROM)
#define MACRO_RAM_EVENTS 1600  // events shared by all RAM slots (~800 keystrokes, 5 bytes each)
// No-delay mode: fixed gap between events, per slot one of these 10 levels in ms (set
// in configure mode with + / -, level 0 is the default; never 0 — hosts drop same-poll
// reports). Max 255.
#define MACRO_GAP_LEVELS {2, 5, 10, 15, 20, 30, 40, 50, 75, 100}
#define DREC_TAP_TERM    200   // double-tap window (ms) for real-delay arming
#define MACRO_FINISH_MS  1000  // a stopped loop finishes its pass if one pass is shorter

// Chord triggers (simultaneous keys) of macro slots run on QMK combos: every key of a
// chord must go down within COMBO_TERM of the first one (strict timer).
#define COMBO_TERM           50
#define COMBO_STRICT_TIMER
#define COMBO_SHOULD_TRIGGER
// Tapped before a key trigger lifts its modifiers, so a lone Alt/GUI tap opens no menu.
#define DUMMY_MOD_NEUTRALIZER_KEYCODE KC_F18

// Persistent macro bank: header + events in the user EEPROM datablock. The board has
// 3072 bytes of (wear-leveled) EEPROM; QMK core + Keychron use < 64 of them.
// Bump the version when the stored layout changes: a mismatch reads back as empty.
#define EECONFIG_USER_DATA_SIZE    2944
#define EECONFIG_USER_DATA_VERSION 0x4D430002

#ifdef RGB_MATRIX_ENABLE
#    define RGB_MATRIX_KEYREACTIVE_ENABLED
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
// Default effect: Solid Reactive Multiwide, color #26D4FF, brightness max, speed 20%
#    define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#    define RGB_MATRIX_DEFAULT_HUE  136
#    define RGB_MATRIX_DEFAULT_SAT  217
#    define RGB_MATRIX_DEFAULT_VAL  255
#    define RGB_MATRIX_DEFAULT_SPD  51
#endif
