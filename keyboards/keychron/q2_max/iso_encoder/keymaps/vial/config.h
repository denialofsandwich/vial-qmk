/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// Custom multi-slot live macro recorder (see keymap.c)
#define MACRO_SLOT_COUNT 10    // slots per bank (RAM and EEPROM)
#define MACRO_RAM_EVENTS 1600  // events shared by all RAM slots (~800 keystrokes, 5 bytes each)
#define MACRO_NODELAY_MS 2     // fixed gap in no-delay mode (never 0 — hosts drop same-poll reports)
#define DREC_TAP_TERM    200   // double-tap window (ms) for real-delay arming

// Persistent macro bank: header + events in the user EEPROM datablock. The board has
// 3072 bytes of (wear-leveled) EEPROM; QMK core + Keychron use < 64 of them.
// Bump the version when the stored layout changes: a mismatch reads back as empty.
#define EECONFIG_USER_DATA_SIZE    2944
#define EECONFIG_USER_DATA_VERSION 0x4D430001

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
