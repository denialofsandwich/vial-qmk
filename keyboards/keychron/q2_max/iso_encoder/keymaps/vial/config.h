/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0x93, 0xE1, 0xE6, 0xFA, 0x10, 0xDA, 0xF4, 0x70}

#define VIAL_UNLOCK_COMBO_ROWS { 0, 1 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 }

// Custom multi-slot live macro recorder (see keymap.c)
#define MACRO_SLOT_COUNT 5
#define MACRO_MAX_EVENTS 64    // ~32 keystrokes (press+release) per slot
#define MACRO_NODELAY_MS 2     // fixed gap in no-delay mode (never 0 — hosts drop same-poll reports)
#define DREC_TAP_TERM    200   // double-tap window (ms) for real-delay arming

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
