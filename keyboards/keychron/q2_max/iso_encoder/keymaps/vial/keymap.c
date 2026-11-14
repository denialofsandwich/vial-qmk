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
#include "nvm_eeprom_eeconfig_internal.h"  // EECONFIG_SIZE, for the EEPROM budget check
#ifdef RGB_MATRIX_ENABLE
#    include "lpm.h"
#endif
#if defined(PROTOCOL_CHIBIOS) && defined(LK_WIRELESS_ENABLE)
#    include <usb_main.h>
#endif

enum layers {
    MAC_BASE,
    WIN_BASE,
    MAC_FN1,
    WIN_FN1,
    FN2,
};

// Based in Keychron's QK_KB custom range (NEW_SAFE_RANGE), after Keychron's own keycodes.
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
    HOLD_LOOP,   // FN2+Backspace: hold down the last key pressed
};
_Static_assert(HOLD_LOOP <= 0x7E1F, "custom keycodes overflow the QK_KB range (0x7E1F)");

// QK_KB is full, so the persistent slots live in QK_USER (free now that VIA is off).
enum persistent_slot_keycodes {
    PSLOT_1 = QK_USER_0,  // FN2+A : persistent (EEPROM) macro slot 1
    PSLOT_2,              // FN2+S
    PSLOT_3,              // FN2+D
    PSLOT_4,              // FN2+F
    PSLOT_5,              // FN2+G
    PSLOT_6,              // FN2+H
    PSLOT_7,              // FN2+J
    PSLOT_8,              // FN2+K
    PSLOT_9,              // FN2+L
    PSLOT_10,             // FN2+Ö : persistent macro slot 10
    CHORD_TOGGLE,         // FN2+Caps: chord triggers on/off (e.g. for games)
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_iso_68(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_MPLY,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,  KC_GRV,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCMMD, KC_LOPTN, KC_LCTL,                                KC_SPC,                                 KC_ROPTN, MO(MAC_FN1),  MO(FN2),  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_BASE] = LAYOUT_iso_68(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_MPLY,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  MO(WIN_FN1),  MO(FN2),  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN1] = LAYOUT_iso_68(
        KC_NUBS,  KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, UG_VALD,  UG_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            UG_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  M0,       _______,  _______,                      _______,
        UG_TOGG,  UG_NEXT,  UG_VALU,  UG_HUEU,  UG_SATU,  UG_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
        _______,  _______,  UG_PREV,  UG_VALD,  UG_HUED,  UG_SATD,  UG_SPDD,  _______,  _______,  _______,  _______,  _______,            _______,  KC_PGUP,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  KC_PGDN,  _______),

    [WIN_FN1] = LAYOUT_iso_68(
        KC_GRV,   KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  UG_VALD,  UG_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT, KC_MUTE,  KC_VOLD,   KC_VOLU,  KC_PSCR,            UG_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        UG_TOGG,  UG_NEXT,  UG_VALU,  UG_HUEU,  UG_SATU,  UG_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
        _______,  KC_INT2,  UG_PREV,  UG_VALD,  UG_HUED,  UG_SATD,  UG_SPDD,  _______,  _______,  _______,  _______,  _______,            _______,  KC_PGUP,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  KC_PGDN,  _______),

    [FN2] = LAYOUT_iso_68(
        KC_TILD,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   HOLD_LOOP,          _______,
        DREC,     MSLOT_1,  MSLOT_2,  MSLOT_3,  MSLOT_4,  MSLOT_5,  MSLOT_6,  MSLOT_7,  MSLOT_8,  MSLOT_9,  MSLOT_10, MS_BTN4,  MS_BTN1,                      MS_WHLU,
        CHORD_TOGGLE, PSLOT_1,  PSLOT_2,  PSLOT_3,  PSLOT_4,  PSLOT_5,  PSLOT_6,  PSLOT_7,  PSLOT_8,  PSLOT_9,  PSLOT_10, MS_BTN5,  MS_BTN2, REPEAT_LOOP,            MS_WHLD,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,            MS_BTN3,  MS_UP,
        RCTRL_LOOP, _______,  _______,                              _______,                                _______,  _______,  _______,  MS_LEFT,  MS_DOWN,  MS_RGHT)
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

static bool     hold_loop_active = false;
static uint16_t hold_keycode     = KC_NO;  // key currently held down by the hold loop
static uint8_t  hold_mods        = 0;

// ---------------------------------------------------------------------------
// Custom multi-slot live macro recorder with two banks of MACRO_SLOT_COUNT slots:
// RAM slots (FN2+Q..P, lost on power-off) and persistent EEPROM slots (FN2+A..Ö).
//
// FN2+Tab   : idle -> arm recording (single tap = no delay, double tap = real delays)
//             triple tap -> configure mode; recording -> stop and save, then configure
//             mode with that slot selected (no keys = erase, back to idle)
//             also the escape hatch out of any state
// FN2+Q..P  : armed -> record into that RAM slot | idle -> play it
// FN2+A..Ö  : idle -> play that EEPROM slot (it can't be recorded into, only copied to)
// any slot  : recording -> play it live (once) AND record a call to it (compose macros)
//             configure mode -> press selects a slot, pressing it again unselects it;
//             another slot copies it (with its settings) there and selects the copy.
//             Copying an empty slot clears the destination. This is how EEPROM slots
//             are filled
//             playing -> interrupt
// FN2+Enter : loop the last played slot, else "repeat last key". Ignored while
//             armed or recording (a loop records nothing and, since a call is only
//             recorded when nothing plays, would swallow every slot press).
//
// Slot settings, edited in configure mode with a non-empty RAM slot selected (EEPROM
// slots are read-only: copy to RAM, edit, copy back). The mode stays on that slot.
// FN2+Enter : cycle repeat mode: off (play once) -> hold (loop while held)
//             -> toggle (loop until pressed again)
// Stopping a loop (slot press, hold release, FN2+Enter) lets a pass shorter than
// MACRO_FINISH_MS finish, so no half-typed text; a longer one stops at once.
// + / -     : (German layout: the keys right of Ü and of '.') no-delay slots only: step
//             the gap between events through MACRO_GAP_LEVELS (default 2 ms; games often
//             need ~20 ms+ to see every key), shown as a bar on the number row
// FN2+Caps  : chord triggers on/off (Caps red = off; on again after a restart). Off,
//             chord keys aren't held back, so games get them without the chord delay
// FN2+Bksp  : capture an alternative trigger. It opens showing the current one; type a
//             new one (release all keys to retype), Bksp alone empties it, FN2+Bksp
//             again applies it (empty = remove the trigger). Either one
//             key with optional modifiers (exact sides: LCtrl != RCtrl), or a chord of
//             2-4 keys pressed together (QMK combos, see COMBO_TERM). Triggers work idle
//             and while recording, match keycodes (not positions) and swallow the key;
//             on a clash RAM slots win over EEPROM ones, then the lower slot.
//
// Each bank is one event pool shared by its slots, so a slot is as long as it needs to
// be. A slot's events are contiguous: freeing a slot compacts the pool, and new content
// always goes to the tail, so the slot being recorded grows by appending.
// An event is a key or a call into another slot (global slot index, so calls are plain
// references — an EEPROM macro calling a RAM slot plays nothing for it after a reboot).
// Playback walks a frame stack, so calls nest and resume their caller.
// Quirk: keys typed while a call plays replay after it, not interleaved.
// ---------------------------------------------------------------------------
#define EVF_PRESSED 0x01
#define EVF_CALL    0x02

typedef struct __attribute__((packed)) {
    uint16_t keycode;   // key: keycode to (un)register; call: target slot (global index)
    uint16_t delay_ms;  // pause that followed this event
    uint8_t  flags;     // EVF_*
} macro_event_t;

// Per-slot settings. An empty slot's settings are all zero.
#define REP_OFF        0
#define REP_TOGGLE     1
#define REP_HOLD       2
#define TRIG_NONE      0
#define TRIG_KEY       1   // keys[0] with exactly `mods` held
#define TRIG_CHORD     2   // keys[] pressed together, sorted, unused entries 0
#define TRIG_MAX_KEYS  4

typedef struct __attribute__((packed)) {
    uint8_t mode;                  // bits 0-1: REP_*, bits 2-3: TRIG_*, bits 4-7: gap level
    uint8_t mods;                  // TRIG_KEY: exact 8-bit modifier mask
    uint8_t keys[TRIG_MAX_KEYS];   // basic keycodes
} macro_cfg_t;

#define CFG_REPEAT(c) ((c)->mode & 0x03)
#define CFG_TRIG(c)   (((c)->mode >> 2) & 0x03)
#define CFG_GAP(c)    ((c)->mode >> 4)

// No-delay gap per level (ms), one level per number-row LED. Never 0: hosts drop
// same-poll reports.
static const uint8_t macro_gap_ms[10] = MACRO_GAP_LEVELS;
_Static_assert(sizeof(macro_gap_ms) == 10, "MACRO_GAP_LEVELS needs 10 entries");

typedef struct __attribute__((packed)) {
    uint16_t    off[MACRO_SLOT_COUNT];  // first event in the pool (only valid when len > 0)
    uint16_t    len[MACRO_SLOT_COUNT];
    uint16_t    realdelay;              // bitmask: playback honors real delays?
    uint16_t    used;                   // events in use, all below this index
    macro_cfg_t cfg[MACRO_SLOT_COUNT];
} macro_hdr_t;

#define MACRO_TOTAL_SLOTS (2 * MACRO_SLOT_COUNT)  // global index: RAM 0..9, EEPROM 10..19
#define MACRO_EE_EVENTS   ((EECONFIG_USER_DATA_SIZE - sizeof(macro_hdr_t)) / sizeof(macro_event_t))
#define MACRO_REC_RESERVE 16  // pool entries kept free so stop_recording can release held keys

// The EEPROM bank is mirrored in RAM; this struct is its exact on-EEPROM image.
typedef struct __attribute__((packed)) {
    macro_hdr_t   hdr;
    macro_event_t ev[MACRO_EE_EVENTS];
} macro_ee_image_t;
_Static_assert(sizeof(macro_ee_image_t) <= EECONFIG_USER_DATA_SIZE, "EEPROM macro image too large");
_Static_assert(EECONFIG_SIZE <= WEAR_LEVELING_LOGICAL_SIZE, "EECONFIG_USER_DATA_SIZE exceeds the EEPROM");
_Static_assert(MACRO_RAM_EVENTS <= 0xFFFF && MACRO_EE_EVENTS <= 0xFFFF, "macro pool index overflow");

static macro_hdr_t      ram_hdr;
static macro_event_t    ram_ev[MACRO_RAM_EVENTS];
static macro_ee_image_t ee_img;

typedef struct {
    macro_hdr_t   *hdr;
    macro_event_t *ev;
    uint16_t       cap;
} macro_bank_t;

static const macro_bank_t macro_banks[2] = {
    {&ram_hdr, ram_ev, MACRO_RAM_EVENTS},
    {&ee_img.hdr, ee_img.ev, MACRO_EE_EVENTS},
};

// ST_CFG: configure mode, nothing selected | ST_CFG_SEL: cfg_slot selected |
// ST_CFG_CAPTURE: typing cfg_slot's alternative trigger
typedef enum { ST_IDLE, ST_ARMED, ST_RECORDING, ST_CFG, ST_CFG_SEL, ST_CFG_CAPTURE } macro_state_t;

static macro_state_t  macro_state     = ST_IDLE;
static bool           armed_realdelay = false;
static uint8_t        active_slot     = 0;               // RAM slot currently being recorded into
static uint8_t        cfg_slot        = 0;               // configure mode: selected slot
static uint32_t       drec_tap_timer  = 0;               // double/triple-tap detection
static uint32_t       last_event_time = 0;               // for inter-event delta
static macro_event_t *last_ev         = NULL;            // event awaiting its delay

typedef struct { uint8_t slot; uint16_t pos; } play_frame_t;
#define MACRO_PLAY_STACK_DEPTH 16                        // caps recursion / cyclic calls

static play_frame_t   play_stack[MACRO_PLAY_STACK_DEPTH];
static uint8_t        play_depth      = 0;               // 0 == not playing
static uint8_t        play_root       = 0;               // slot to restart from when looping
static bool           play_loop       = false;
static uint32_t       play_timer      = 0;               // when current step's wait started
static uint16_t       play_wait       = 0;                // ms to wait before processing top frame
static uint32_t       play_done_ms    = 0;               // macro time of the finished steps
static uint32_t       play_total_ms   = 0;               // macro time of a full run (progress bar)
static int8_t         last_macro_slot = -1;              // last slot played (for FN2+Enter loop)
static int8_t         hold_slot       = -1;              // hold-repeat slot whose trigger is down

// Keys playback registered, so teardown releases exactly those (see release_play_held).
#define MACRO_PLAY_HELD_MAX 16
static uint16_t       play_held[MACRO_PLAY_HELD_MAX];
static uint8_t        play_nheld         = 0;
static bool           play_held_overflow = false;

static inline int8_t slot_index(uint16_t keycode) {
    if (keycode >= MSLOT_1 && keycode <= MSLOT_10) return (int8_t)(keycode - MSLOT_1);
    if (keycode >= PSLOT_1 && keycode <= PSLOT_10) return (int8_t)(MACRO_SLOT_COUNT + keycode - PSLOT_1);
    return -1;
}

static inline const macro_bank_t *slot_bank(uint8_t s) { return &macro_banks[s / MACRO_SLOT_COUNT]; }
static inline uint8_t  slot_local(uint8_t s) { return s % MACRO_SLOT_COUNT; }
static inline bool     slot_is_ee(uint8_t s) { return s >= MACRO_SLOT_COUNT; }
static inline uint16_t slot_len(uint8_t s) { return slot_bank(s)->hdr->len[slot_local(s)]; }

static inline macro_event_t *slot_ev(uint8_t s, uint16_t pos) {
    const macro_bank_t *b = slot_bank(s);
    return &b->ev[b->hdr->off[slot_local(s)] + pos];
}

static inline bool slot_realdelay(uint8_t s) {
    return (slot_bank(s)->hdr->realdelay >> slot_local(s)) & 1;
}

static void set_slot_realdelay(uint8_t s, bool on) {
    uint16_t bit = (uint16_t)1 << slot_local(s);
    if (on) slot_bank(s)->hdr->realdelay |= bit;
    else    slot_bank(s)->hdr->realdelay &= ~bit;
}

static inline macro_cfg_t *slot_cfg(uint8_t s) { return &slot_bank(s)->hdr->cfg[slot_local(s)]; }

// The fixed wait of a no-delay step (and of a call before its callee runs).
static inline uint16_t slot_gap(uint8_t s) {
    uint8_t lv = CFG_GAP(slot_cfg(s));
    return macro_gap_ms[lv < 10 ? lv : 9];
}

// Chord triggers as QMK combos, one per slot (combo index == global slot index). The
// table lives in RAM and is rebuilt whenever a trigger changes; a slot without a chord
// gets an empty key list, which never matches. KC_NO combos report through
// process_combo_event(), which plays the slot.
static uint16_t chord_keys[MACRO_TOTAL_SLOTS][TRIG_MAX_KEYS + 1];  // COMBO_END-terminated
#define CHORD(i) {.keys = chord_keys[i], .keycode = KC_NO}
combo_t key_combos[MACRO_TOTAL_SLOTS] = {
    CHORD(0),  CHORD(1),  CHORD(2),  CHORD(3),  CHORD(4),  CHORD(5),  CHORD(6),
    CHORD(7),  CHORD(8),  CHORD(9),  CHORD(10), CHORD(11), CHORD(12), CHORD(13),
    CHORD(14), CHORD(15), CHORD(16), CHORD(17), CHORD(18), CHORD(19),
};
_Static_assert(MACRO_TOTAL_SLOTS == 20, "update key_combos[]");

// A chord equal to an earlier slot's (RAM before EEPROM) is left out, so that one wins.
static void rebuild_chords(void) {
    for (uint8_t s = 0; s < MACRO_TOTAL_SLOTS; s++) {
        const macro_cfg_t *c = slot_cfg(s);
        bool on = CFG_TRIG(c) == TRIG_CHORD;
        for (uint8_t t = 0; on && t < s; t++) {
            const macro_cfg_t *o = slot_cfg(t);
            if (CFG_TRIG(o) == TRIG_CHORD && !memcmp(o->keys, c->keys, TRIG_MAX_KEYS)) on = false;
        }
        for (uint8_t i = 0; i < TRIG_MAX_KEYS; i++) chord_keys[s][i] = on ? c->keys[i] : COMBO_END;
        chord_keys[s][TRIG_MAX_KEYS] = COMBO_END;
        key_combos[s] = (combo_t)CHORD(s);  // also resets the combo's match state
    }
}

// Free a slot's events and close the gap. Offsets of later slots shift down, so
// pointers into the pool are stale afterwards (play frames hold slot+pos, not pointers).
static void slot_clear(uint8_t s) {
    const macro_bank_t *b = slot_bank(s);
    macro_hdr_t        *h = b->hdr;
    uint8_t             l = slot_local(s);
    uint16_t len = h->len[l], off = h->off[l];
    h->len[l] = 0;
    h->off[l] = 0;
    if (len == 0) return;
    memmove(&b->ev[off], &b->ev[off + len], (h->used - off - len) * sizeof(macro_event_t));
    for (uint8_t i = 0; i < MACRO_SLOT_COUNT; i++) {
        if (h->len[i] && h->off[i] > off) h->off[i] -= len;
    }
    h->used -= len;
}

// In chunks: eeprom_update_block() puts a copy of the whole range on the (2 KB) stack.
// Unchanged chunks are skipped there, so only what moved gets written.
#define MACRO_EE_CHUNK 32
static void ee_save(void) {
    const uint8_t *img = (const uint8_t *)&ee_img;
    uint32_t       n   = sizeof(macro_hdr_t) + ee_img.hdr.used * sizeof(macro_event_t);
    for (uint32_t o = 0; o < n; o += MACRO_EE_CHUNK) {
        eeconfig_update_user_datablock(img + o, o, MIN(n - o, (uint32_t)MACRO_EE_CHUNK));
    }
}

static void ee_load(void) {
    eeconfig_read_user_datablock(&ee_img, 0, sizeof(ee_img));
    macro_hdr_t *h  = &ee_img.hdr;
    bool         ok = h->used <= MACRO_EE_EVENTS;
    for (uint8_t i = 0; i < MACRO_SLOT_COUNT; i++) {
        if (h->len[i] && (uint32_t)h->off[i] + h->len[i] > h->used) ok = false;
    }
    if (!ok) memset(&ee_img, 0, sizeof(ee_img));
    rebuild_chords();
}

// Copy src's events to the tail of dst's bank (an empty src clears dst). False = no
// room; dst is left untouched then.
static bool copy_slot(uint8_t src, uint8_t dst) {
    if (src == dst) return true;
    const macro_bank_t *db = slot_bank(dst);
    uint8_t             dl = slot_local(dst);
    uint16_t            n  = slot_len(src);
    if ((uint32_t)db->hdr->used - db->hdr->len[dl] + n > db->cap) return false;
    bool        rd  = slot_realdelay(src);
    macro_cfg_t cfg = *slot_cfg(src);
    slot_clear(dst);
    if (n) {
        // Resolve src only now: clearing dst may have compacted it (same bank). It sits
        // below the tail, so the ranges never overlap.
        memcpy(&db->ev[db->hdr->used], slot_ev(src, 0), n * sizeof(macro_event_t));
        db->hdr->off[dl] = db->hdr->used;
        db->hdr->len[dl] = n;
        db->hdr->used += n;
    }
    set_slot_realdelay(dst, rd);
    *slot_cfg(dst) = cfg;
    if (slot_is_ee(dst)) ee_save();
    rebuild_chords();
    return true;
}

static uint16_t clamp_u16(uint32_t v) { return v > 0xFFFF ? 0xFFFF : (uint16_t)v; }

// The slot being recorded is still being written, so playing it would feed the
// recording into itself. Also checked per call: a callee may call back in.
static inline bool call_blocked(uint8_t slot) {
    return macro_state == ST_RECORDING && slot == active_slot;
}

// Whether playback would enter this call (ignoring the stack depth cap).
static inline bool call_runs(uint8_t target) {
    return target < MACRO_TOTAL_SLOTS && slot_len(target) > 0 && !call_blocked(target);
}

static bool is_recordable(uint16_t keycode, keyrecord_t *record) {
    if (record->event.type != KEY_EVENT) return false;   // skip encoder events
    if (keycode == DREC || slot_index(keycode) >= 0) return false;
    switch (keycode) {
        case M0:
        case RCTRL_LOOP:
        case REPEAT_LOOP:
        case HOLD_LOOP:
        case CHORD_TOGGLE:
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

// An event's delay is the pause that followed it, which is only known once the next
// event arrives (or recording stops) — so it is written back then, not on append.
static void close_interval(void) {
    if (last_ev) last_ev->delay_ms = clamp_u16(timer_elapsed32(last_event_time));
}

static void stop_recording(void) {
    if (macro_state != ST_RECORDING) return;
    close_interval();
    last_ev = NULL;
    uint8_t s = active_slot;

    // Release keys still held at stop, so playback never sticks one down. close_interval()
    // above put the pause on the last real event, so a key held until the stop key stays
    // held that long on playback. Appends use the pool space append_event() kept free.
    uint16_t held[MACRO_REC_RESERVE];
    uint8_t  nheld = 0;
    for (uint16_t i = 0; i < ram_hdr.len[s]; i++) {
        macro_event_t *e = slot_ev(s, i);
        if (e->flags & EVF_CALL) continue;
        if (e->flags & EVF_PRESSED) {
            bool found = false;
            for (uint8_t h = 0; h < nheld; h++) if (held[h] == e->keycode) { found = true; break; }
            if (!found && nheld < MACRO_REC_RESERVE) held[nheld++] = e->keycode;
        } else {
            for (uint8_t h = 0; h < nheld; h++) if (held[h] == e->keycode) { held[h] = held[--nheld]; break; }
        }
    }
    for (uint8_t h = 0; h < nheld && ram_hdr.used < MACRO_RAM_EVENTS; h++) {
        macro_event_t *e = &ram_ev[ram_hdr.used++];
        e->keycode       = held[h];
        e->delay_ms      = 0;
        e->flags         = 0;
        ram_hdr.len[s]++;
    }
    if (ram_hdr.len[s] == 0) {                    // nothing typed -> slot erased
        ram_hdr.off[s] = 0;
        memset(&ram_hdr.cfg[s], 0, sizeof(macro_cfg_t));
        rebuild_chords();
    }
    macro_state = ST_IDLE;
}

// The recorded slot sits at the pool tail, so an event is one more entry there. A full
// pool ends the recording.
static macro_event_t *append_event(uint8_t flags) {
    close_interval();
    if (ram_hdr.used + MACRO_REC_RESERVE >= MACRO_RAM_EVENTS) { stop_recording(); return NULL; }
    macro_event_t *e = &ram_ev[ram_hdr.used++];
    ram_hdr.len[active_slot]++;
    e->delay_ms      = 0;
    e->flags         = flags;
    last_ev          = e;
    last_event_time  = timer_read32();
    return e;
}

static void record_call(uint8_t target) {
    macro_event_t *e = append_event(EVF_CALL);
    if (!e) return;
    e->keycode = target;
}

static void record_event(uint16_t keycode, bool pressed) {
    macro_event_t *e = append_event(pressed ? EVF_PRESSED : 0);
    if (!e) return;
    e->keycode = keycode;
}

static void start_recording(uint8_t slot) {
    slot_clear(slot);
    ram_hdr.off[slot] = ram_hdr.used;
    set_slot_realdelay(slot, armed_realdelay);
    active_slot     = slot;
    last_ev         = NULL;
    last_event_time = timer_read32();
    macro_state     = ST_RECORDING;
}

// Macro time of one run of a slot, mirroring the waits matrix_scan_user schedules:
// a key waits w (real delay or the slot's gap); a call waits the gap, runs
// the callee, then waits its own w. Memoized per slot, since calls may fan out; a slot
// that (indirectly) calls itself counts as 0 there, where playback would run until
// the stack depth cap.
static uint32_t dur_memo[MACRO_TOTAL_SLOTS];
static uint8_t  dur_state[MACRO_TOTAL_SLOTS];  // 0 = unknown, 1 = computing, 2 = done

static uint32_t slot_duration(uint8_t s) {
    if (dur_state[s] == 2) return dur_memo[s];
    if (dur_state[s] == 1) return 0;
    dur_state[s] = 1;
    bool     rd    = slot_realdelay(s);
    uint32_t total = 0;
    for (uint16_t i = 0; i < slot_len(s); i++) {
        macro_event_t *e = slot_ev(s, i);
        uint16_t       w = rd ? e->delay_ms : slot_gap(s);
        if ((e->flags & EVF_CALL) && call_runs((uint8_t)e->keycode)) {
            total += slot_gap(s) + slot_duration((uint8_t)e->keycode);
        }
        total += w;
    }
    dur_memo[s]  = total;
    dur_state[s] = 2;
    return total;
}

// The Mac layer's Option/Cmd are Keychron keycodes that only process_record turns into
// real modifiers; playback registers codes directly, so map them here (else e.g. the
// German Mac '@' = Option+L replays as a bare 'l').
static uint16_t play_keycode(uint16_t keycode) {
    switch (keycode) {
        case KC_LOPTN: return KC_LOPT;
        case KC_ROPTN: return KC_ROPT;
        case KC_LCMMD: return KC_LCMD;
        case KC_RCMMD: return KC_RCMD;
    }
    return keycode;
}

// Not clear_keyboard(): that would also drop keys and mods the user physically holds
// while a called macro ends (and wipe mousekeys).
static void release_play_held(void) {
    if (play_held_overflow) clear_keyboard();            // fallback: never leave a key stuck
    else while (play_nheld) unregister_code16(play_keycode(play_held[--play_nheld]));
    play_nheld         = 0;
    play_held_overflow = false;
}

static void start_playback(uint8_t slot, bool loop) {
    if (slot_len(slot) == 0) return;
    if (play_depth > 0) release_play_held();             // restarting over a live playback
    play_stack[0].slot = slot;
    play_stack[0].pos  = 0;
    play_depth         = 1;
    play_root          = slot;
    play_loop          = loop;
    play_wait          = 0;
    play_timer         = timer_read32();
    play_done_ms       = 0;
    memset(dur_state, 0, sizeof(dur_state));
    play_total_ms      = slot_duration(slot);
}

static void stop_playback(void) {
    play_depth = 0;
    play_loop  = false;
    release_play_held();
    // A call's live duration must not inflate the interval being recorded: the replay
    // re-runs the call, so that time is spent there too.
    if (macro_state == ST_RECORDING) last_event_time = timer_read32();
}

// End a playback on request: a looping pass shorter than MACRO_FINISH_MS (no-delay
// macros, short real-delay ones) runs to its end, anything else stops right now.
static void stop_playback_soft(void) {
    if (play_depth > 0 && play_loop && play_total_ms < MACRO_FINISH_MS) play_loop = false;
    else                                                                stop_playback();
}

static void stop_hold_loop(void) {
    if (hold_keycode != KC_NO) {
        unregister_code16(hold_keycode);
        unregister_mods(hold_mods);
    }
    hold_keycode     = KC_NO;
    hold_mods        = 0;
    hold_loop_active = false;
}

// A slot was triggered: its key, its alternative trigger, or its chord.
static void macro_trigger_press(uint8_t s) {
    stop_hold_loop();
    if (macro_state == ST_RECORDING) {
        // Run the slot live *and* record a call to it, once whatever its repeat mode
        // (a loop can't be recorded). last_macro_slot stays put, so a later FN2+Enter
        // loops the macro being built.
        if (play_depth == 0 && call_runs(s)) {
            record_call(s);                                  // first: it stamps last_event_time
            start_playback(s, false);
        }
        return;
    }
    if (play_depth > 0) {
        if (CFG_REPEAT(slot_cfg(s)) == REP_HOLD && play_root == s && !play_loop) {
            play_loop = true;                                // re-pressed while its last pass runs
            hold_slot = s;
        } else {                                             // press during playback -> interrupt
            stop_playback_soft();
            repeat_loop_active = false;
        }
        return;
    }
    if (slot_len(s) == 0) return;
    uint8_t rep     = CFG_REPEAT(slot_cfg(s));
    last_macro_slot = s;
    start_playback(s, rep != REP_OFF);
    hold_slot = rep == REP_HOLD ? (int8_t)s : -1;
}

// Hold repeat: released -> stop the loop.
static void macro_trigger_release(uint8_t s) {
    if (hold_slot != (int8_t)s) return;
    hold_slot = -1;
    if (play_depth > 0 && play_root == s && play_loop) stop_playback_soft();
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (combo_index >= MACRO_TOTAL_SLOTS) return;
    if (pressed) macro_trigger_press((uint8_t)combo_index);
    else         macro_trigger_release((uint8_t)combo_index);
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    if (macro_state != ST_IDLE && macro_state != ST_RECORDING) return false;
    return combo_index < MACRO_TOTAL_SLOTS && slot_len((uint8_t)combo_index) > 0 &&
           !call_blocked((uint8_t)combo_index);
}

// Key triggers (one key + exact modifiers). The trigger's modifiers are lifted from the
// host while the macro plays (else it would type with Ctrl held), but still count as
// held for matching, so the trigger can be pressed again with them down. They come
// back when a modifier key is released or another key is pressed.
static uint8_t  sup_mods     = 0;      // modifiers lifted by a key trigger, still physically held
static bool     trig_down    = false;  // a key trigger is held at trig_pos
static keypos_t trig_pos;
static uint8_t  trig_slot    = 0;

static int8_t find_key_trigger(uint8_t kc, uint8_t mods) {
    for (uint8_t s = 0; s < MACRO_TOTAL_SLOTS; s++) {        // RAM first, then EEPROM
        const macro_cfg_t *c = slot_cfg(s);
        if (CFG_TRIG(c) == TRIG_KEY && c->keys[0] == kc && c->mods == mods &&
            slot_len(s) > 0 && !call_blocked(s)) {
            return (int8_t)s;
        }
    }
    return -1;
}

// Recorded as modifier key events, so a recording replays what the host saw.
static void record_mods(uint8_t mods, bool pressed) {
    for (uint8_t i = 0; i < 8; i++) {
        if (mods & (1 << i)) record_event(KC_LEFT_CTRL + i, pressed);
    }
}

static void restore_sup_mods(void) {
    if (!sup_mods) return;
    if (macro_state == ST_RECORDING) record_mods(sup_mods, true);
    add_mods(sup_mods);
    sup_mods = 0;
}

// Trigger capture (ST_CFG_CAPTURE). A press while nothing captured is held starts over.
#define CAP_MAX 8
static struct {
    uint8_t  mods;
    uint8_t  keys[TRIG_MAX_KEYS];   // non-modifier keys, in press order
    uint8_t  nkeys;
    bool     overflow;              // more keys than a trigger can hold
    keypos_t pos[CAP_MAX];          // every captured key, modifiers too (for the LEDs)
    uint8_t  npos;
    keypos_t held[CAP_MAX];         // captured keys still down
    uint8_t  nheld;
    uint32_t first, last;           // press times of the first and last non-modifier key
} cap;

#define CAP_INVALID 0xFF

// TRIG_NONE (nothing typed), TRIG_KEY, TRIG_CHORD or CAP_INVALID.
static uint8_t cap_kind(void) {
    if (cap.nkeys == 0 && !cap.mods && !cap.overflow) return TRIG_NONE;
    if (cap.overflow) return CAP_INVALID;
    if (cap.nkeys == 1) return TRIG_KEY;
    if (cap.nkeys >= 2 && !cap.mods && cap.last - cap.first <= COMBO_TERM) return TRIG_CHORD;
    return CAP_INVALID;                    // modifiers only, modifiers + chord, too slow
}

static void cap_press(uint8_t kc, keypos_t pos) {
    if (cap.nheld == 0) memset(&cap, 0, sizeof(cap));
    if (cap.nheld < CAP_MAX) cap.held[cap.nheld++] = pos;
    if (cap.npos < CAP_MAX) cap.pos[cap.npos++] = pos;
    else                    cap.overflow = true;
    if (IS_MODIFIER_KEYCODE(kc)) {
        cap.mods |= MOD_BIT(kc);
        return;
    }
    for (uint8_t i = 0; i < cap.nkeys; i++) {
        if (cap.keys[i] == kc) return;     // same keycode on another key
    }
    if (cap.nkeys == TRIG_MAX_KEYS) { cap.overflow = true; return; }
    uint32_t now = timer_read32();
    if (cap.nkeys == 0) cap.first = now;
    cap.last              = now;
    cap.keys[cap.nkeys++] = (uint8_t)kc;
}

// False if this key's press wasn't captured (it went to the host before capture began).
static bool cap_release(keypos_t pos) {
    for (uint8_t i = 0; i < cap.nheld; i++) {
        if (KEYEQ(cap.held[i], pos)) {
            cap.held[i] = cap.held[--cap.nheld];
            return true;
        }
    }
    return false;
}

// FN2+Bksp in capture: store the trigger (or remove it). An invalid capture is refused.
// A key showing `kc` (Mac Option/Cmd as modifiers), searched on the base layer first so
// the usual key lights, then on the others (e.g. F1 lives on FN2 only).
static bool find_key_pos(uint8_t kc, keypos_t *out) {
    uint8_t base = get_highest_layer(default_layer_state);
    for (uint8_t i = 0; i <= FN2; i++) {
        uint8_t layer = i == 0 ? base : (i == base ? 0 : i);
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            for (uint8_t c = 0; c < MATRIX_COLS; c++) {
                keypos_t pos = MAKE_KEYPOS(r, c);
                if (play_keycode(keymap_key_to_keycode(layer, pos)) == kc) {
                    *out = pos;
                    return true;
                }
            }
        }
    }
    return false;
}

// Capture opens on the slot's current trigger, so it can be looked at and kept as is.
// The timestamps are equal, so a stored chord classifies as a chord again.
static void cap_load(const macro_cfg_t *c) {
    memset(&cap, 0, sizeof(cap));
    uint8_t trig = CFG_TRIG(c);
    if (trig == TRIG_NONE) return;
    keypos_t pos;
    for (uint8_t i = 0; i < 8 && trig == TRIG_KEY; i++) {
        if (!(c->mods & (1 << i))) continue;
        cap.mods |= 1 << i;
        if (cap.npos < CAP_MAX && find_key_pos(KC_LEFT_CTRL + i, &pos)) cap.pos[cap.npos++] = pos;
    }
    for (uint8_t i = 0; i < TRIG_MAX_KEYS && c->keys[i]; i++) {
        cap.keys[cap.nkeys++] = c->keys[i];
        if (cap.npos < CAP_MAX && find_key_pos(c->keys[i], &pos)) cap.pos[cap.npos++] = pos;
    }
}

static void cap_apply(void) {
    uint8_t kind = cap_kind();
    if (kind == CAP_INVALID) return;
    macro_cfg_t *c = slot_cfg(cfg_slot);
    c->mode        = (c->mode & ~0x0C) | (kind << 2);  // keep repeat mode and gap
    c->mods        = kind == TRIG_KEY ? cap.mods : 0;
    memset(c->keys, 0, sizeof(c->keys));
    for (uint8_t i = 0; i < cap.nkeys && kind != TRIG_NONE; i++) {
        // chords sorted, so equal chords compare equal (rebuild_chords)
        uint8_t j = i;
        while (j > 0 && c->keys[j - 1] > cap.keys[i]) { c->keys[j] = c->keys[j - 1]; j--; }
        c->keys[j] = cap.keys[i];
    }
    memset(&cap, 0, sizeof(cap));
    rebuild_chords();
    macro_state = ST_CFG_SEL;
}

// Settings are editable on non-empty RAM slots only.
static inline bool cfg_editable(void) {
    return macro_state == ST_CFG_SEL && !slot_is_ee(cfg_slot) && slot_len(cfg_slot) > 0;
}

// Chord keys are held back until the chord is ruled out (up to COMBO_TERM). A release
// before that sends the press and the release back to back, and games that sample the
// key state once per frame miss such a sub-millisecond tap. So each basic key's release
// is held back for as long as the key was physically down, minus how long the host
// already saw it down: the host gets the real hold time, shifted by the chord delay.
#define HELD_MAX 8
typedef struct {
    keypos_t       pos;
    uint8_t        kc;
    bool           used;
    uint16_t       pressed_at;   // physical press (event time)
    uint16_t       sent_at;      // when the press reached the host
    deferred_token release;      // pending held-back release, else INVALID_DEFERRED_TOKEN
} held_key_t;
static held_key_t held_keys[HELD_MAX];

static held_key_t *held_find(keypos_t pos) {
    for (uint8_t i = 0; i < HELD_MAX; i++) {
        if (held_keys[i].used && KEYEQ(held_keys[i].pos, pos)) return &held_keys[i];
    }
    return NULL;
}

// Send a held-back release now (also what is recorded, so recordings keep the hold).
static void held_send_release(held_key_t *h) {
    if (h->release != INVALID_DEFERRED_TOKEN) cancel_deferred_exec(h->release);
    if (macro_state == ST_RECORDING) record_event(h->kc, false);
    unregister_code(h->kc);
    h->used = false;
}

static uint32_t held_release_cb(uint32_t trigger_time, void *arg) {
    held_key_t *h = (held_key_t *)arg;
    h->release    = INVALID_DEFERRED_TOKEN;              // fired: nothing left to cancel
    held_send_release(h);
    return 0;
}

static void held_on_press(uint8_t kc, keyrecord_t *record) {
    held_key_t *h = held_find(record->event.key);
    if (h) held_send_release(h);                         // re-pressed before its release went out
    for (uint8_t i = 0; i < HELD_MAX; i++) {
        if (held_keys[i].used) continue;
        held_keys[i] = (held_key_t){.pos = record->event.key, .kc = kc, .used = true,
                                    .pressed_at = record->event.time, .sent_at = timer_read(),
                                    .release = INVALID_DEFERRED_TOKEN};
        return;
    }                                                    // table full: untracked, released as usual
}

// True if the release was held back (the caller swallows it).
static bool held_on_release(keyrecord_t *record) {
    held_key_t *h = held_find(record->event.key);
    if (!h || h->release != INVALID_DEFERRED_TOKEN) return false;
    int32_t short_by = (int32_t)TIMER_DIFF_16(record->event.time, h->pressed_at) -
                       (int32_t)TIMER_DIFF_16(timer_read(), h->sent_at);
    if (short_by > 0) {
        h->release = defer_exec((uint32_t)short_by, held_release_cb, h);
        if (h->release != INVALID_DEFERRED_TOKEN) return true;
    }
    h->used = false;                                     // released on time: handled normally
    return false;
}

void keyboard_post_init_user(void) {
    ee_load();
}

// EEPROM reset (also on first boot): the datablock was just zeroed, drop the mirror too.
void eeconfig_init_user(void) {
    memset(&ee_img, 0, sizeof(ee_img));
    rebuild_chords();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int8_t   slot    = slot_index(keycode);
    bool     pressed = record->event.pressed;
    uint16_t kc      = play_keycode(keycode);  // Mac Option/Cmd as real modifiers

    if (keycode == DREC) {
        if (pressed) {
            stop_hold_loop();
            if (play_depth > 0) stop_playback();
            // The "repeat last key" loop runs with play_depth == 0 and re-enters
            // process_record_user, so leaving it on would flood a fresh recording.
            repeat_loop_active = false;
            if (macro_state == ST_RECORDING) {
                stop_recording();
                if (slot_len(active_slot) > 0) {   // saved -> configure it right away
                    cfg_slot    = active_slot;
                    macro_state = ST_CFG_SEL;
                }
            } else if (macro_state == ST_ARMED) {
                if (timer_elapsed32(drec_tap_timer) >= DREC_TAP_TERM) {
                    macro_state = ST_IDLE;         // settled press while armed -> abort
                } else if (!armed_realdelay) {
                    armed_realdelay = true;        // quick double tap -> measure real delays
                } else {
                    macro_state = ST_CFG;          // quick triple tap -> configure mode
                }
            } else if (macro_state == ST_CFG || macro_state == ST_CFG_SEL || macro_state == ST_CFG_CAPTURE) {
                macro_state = ST_IDLE;             // leave configure mode
            } else {
                armed_realdelay = false;           // single tap -> arm (no delay)
                macro_state     = ST_ARMED;
            }
            drec_tap_timer = timer_read32();
        }
        return false;
    }

    // Releases that belong to an earlier key trigger, in any state.
    if (!pressed && record->event.type == KEY_EVENT) {
        if (trig_down && KEYEQ(record->event.key, trig_pos)) {
            trig_down = false;
            macro_trigger_release(trig_slot);
            return false;
        }
        if (IS_MODIFIER_KEYCODE(kc) && (sup_mods & MOD_BIT(kc))) {
            sup_mods &= ~MOD_BIT(kc);              // already lifted (and recorded as released)
            return true;
        }
        if (held_on_release(record)) return false; // sent later, see held_keys
    }

    if (macro_state == ST_CFG_CAPTURE && record->event.type == KEY_EVENT) {
        if (keycode == HOLD_LOOP) {
            if (pressed) cap_apply();
            return false;
        }
        if (kc == KC_BSPC) {                       // Bksp: empty the capture (no trigger)
            if (pressed) memset(&cap, 0, sizeof(cap));
            return false;
        }
        if (IS_QK_MOMENTARY(keycode) || IS_QK_LAYER_TAP(keycode) || IS_QK_TO(keycode) ||
            IS_QK_TOGGLE_LAYER(keycode) || IS_QK_LAYER_MOD(keycode)) {
            return true;                           // FN2 must work to reach FN2+Bksp
        }
        if (pressed) {
            if (IS_BASIC_KEYCODE(kc) || IS_MODIFIER_KEYCODE(kc)) cap_press((uint8_t)kc, record->event.key);
            return false;                          // other keys: ignored
        }
        return !cap_release(record->event.key);    // pass releases of keys pressed before
    }

    // Configure: + / - step the selected no-delay slot's gap. Matched by the key's base
    // layer keycode, so they also work with FN2 held (+ is a mouse button there).
    if (macro_state == ST_CFG_SEL && record->event.type == KEY_EVENT) {
        uint16_t base_kc = keymap_key_to_keycode(get_highest_layer(default_layer_state), record->event.key);
        if (base_kc == KC_RBRC || base_kc == KC_SLSH) {        // German + / -
            if (pressed && cfg_editable() && !slot_realdelay(cfg_slot)) {
                macro_cfg_t *c  = slot_cfg(cfg_slot);
                uint8_t      lv = CFG_GAP(c);
                if (base_kc == KC_RBRC && lv < 9) lv++;
                if (base_kc == KC_SLSH && lv > 0) lv--;
                c->mode = (c->mode & 0x0F) | (lv << 4);
            }
            return false;
        }
    }

    if (slot >= 0) {
        if (pressed) {
            if (macro_state == ST_ARMED) {
                stop_hold_loop();
                if (!slot_is_ee((uint8_t)slot)) start_recording((uint8_t)slot);  // EEPROM: copy only
            } else if (macro_state == ST_CFG) {
                cfg_slot    = (uint8_t)slot;
                macro_state = ST_CFG_SEL;
            } else if (macro_state == ST_CFG_SEL) {
                if ((uint8_t)slot == cfg_slot) {
                    macro_state = ST_CFG;          // same slot again -> unselect
                } else if (copy_slot(cfg_slot, (uint8_t)slot)) {
                    cfg_slot = (uint8_t)slot;      // copied -> the copy is selected
                }                                  // no room: nothing changed, keep the source
            } else {                               // ST_IDLE / ST_RECORDING
                macro_trigger_press((uint8_t)slot);
            }
        } else {
            macro_trigger_release((uint8_t)slot);
        }
        return false;
    }

    // Key triggers: a basic key with exactly the trigger's modifiers.
    if (pressed && IS_BASIC_KEYCODE(kc) && record->event.type == KEY_EVENT &&
        (macro_state == ST_IDLE || macro_state == ST_RECORDING)) {
        int8_t t = find_key_trigger((uint8_t)kc, get_mods() | sup_mods);
        if (t >= 0) {
            uint8_t live = get_mods();             // the trigger's mods not yet lifted
            if (live) {
                neutralize_flashing_modifiers(live);
                del_mods(live);
                send_keyboard_report();
                if (macro_state == ST_RECORDING) record_mods(live, false);
                sup_mods |= live;
            }
            trig_down = true;
            trig_pos  = record->event.key;
            trig_slot = (uint8_t)t;
            macro_trigger_press((uint8_t)t);
            return false;
        }
        if (play_depth == 0) restore_sup_mods();   // e.g. Ctrl still held, now Ctrl+C
    }

    if (pressed && IS_BASIC_KEYCODE(kc) && record->event.type == KEY_EVENT) {
        held_on_press((uint8_t)kc, record);
    }

    // A normal key press becomes the new "last action" for FN2+Enter. is_recordable()
    // excludes layer keys, so holding FN2 to reach Enter doesn't reset this.
    if (pressed && is_recordable(keycode, record)) {
        last_macro_slot = -1;
    }

    if (macro_state == ST_RECORDING && is_recordable(keycode, record)) {
        record_event(keycode, pressed);
        return true;  // also type the key live
    }

    switch (keycode) {
        // Mac Layer only: Remap Ctrl+Left/Right/Backspace to Alt(option)+Left/Right/Backspace
        case KC_LEFT:
        case KC_RIGHT:
        case KC_BSPC: {
            if (record->event.pressed && get_highest_layer(default_layer_state) == MAC_BASE) {
                uint8_t mods      = get_mods();
                uint8_t ctrl_mods = mods & MOD_MASK_GUI;
                if (ctrl_mods && !(mods & ~MOD_MASK_GUI)) {
                    del_mods(ctrl_mods);
                    tap_code16(LALT(keycode));
                    add_mods(ctrl_mods);
                    return false;
                }
            }
            break;
        }
        case M0:
            if (record->event.pressed) {
                SEND_STRING(M0_SEQ);
            }
            return false;
        case CHORD_TOGGLE:
            if (record->event.pressed) combo_toggle();  // off also flushes held-back keys
            return false;
        case RCTRL_LOOP:
            if (record->event.pressed) {
                rctrl_loop_active = !rctrl_loop_active;
            }
            return false;
        case REPEAT_LOOP:
            if (record->event.pressed) {
                if (cfg_editable()) {                      // configure: cycle repeat mode
                    macro_cfg_t *c = slot_cfg(cfg_slot);
                    c->mode        = (c->mode & ~0x03) | ((CFG_REPEAT(c) + 2) % 3);  // off -> hold -> toggle
                    return false;
                }
                if (macro_state != ST_IDLE) return false;  // see the header comment
                stop_hold_loop();  // mutually exclusive with the hold loop
                repeat_loop_active = !repeat_loop_active;
                if (repeat_loop_active) {
                    if (last_macro_slot >= 0 && slot_len((uint8_t)last_macro_slot) > 0) {
                        start_playback((uint8_t)last_macro_slot, true);
                    }
                    // else: "repeat last key" handled in matrix_scan_user
                } else {
                    stop_playback_soft();
                }
            }
            return false;
        case HOLD_LOOP:
            if (record->event.pressed) {
                if (macro_state == ST_CFG || macro_state == ST_CFG_SEL) {
                    if (cfg_editable()) {                  // configure: capture a trigger
                        cap_load(slot_cfg(cfg_slot));
                        macro_state = ST_CFG_CAPTURE;
                    }
                    return false;
                }
                if (hold_loop_active) {
                    stop_hold_loop();
                } else {
                    // mutually exclusive with the repeat / macro loop
                    if (repeat_loop_active) { repeat_loop_active = false; stop_playback(); }
                    hold_keycode = get_last_keycode();
                    hold_mods    = get_last_mods();
                    if (hold_keycode != KC_NO) {
                        hold_loop_active = true;
                        register_mods(hold_mods);
                        register_code16(hold_keycode);
                    }
                }
            }
            return false;
    }
    return true;
}

// Keep our own keycodes out of the Repeat Key memory, so the loops repeat the last
// "real" key instead of the toggle itself.
bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    // While a loop runs, keep its target locked to the key pressed before it started.
    if (repeat_loop_active || hold_loop_active) return false;
    if (keycode == DREC || slot_index(keycode) >= 0) return false;
    switch (keycode) {
        case M0:
        case RCTRL_LOOP:
        case REPEAT_LOOP:
        case HOLD_LOOP:
        case CHORD_TOGGLE:
            return false;
    }
    return true;
}

// Wake the host when a loop or macro runs while USB is suspended. Without this,
// Keychron's usb_remote_wakeup() loop (transport.c) never calls matrix_scan_user(),
// so playback stalls until a physical key is pressed.
#if defined(PROTOCOL_CHIBIOS) && defined(LK_WIRELESS_ENABLE)
void suspend_power_down_user(void) {
    static uint32_t wakeup_timer = 0;
    if ((play_depth > 0 || repeat_loop_active || rctrl_loop_active || hold_loop_active)
        && USB_DRIVER.state == USB_SUSPENDED
        && timer_elapsed32(wakeup_timer) > 500) {
        wakeup_timer = timer_read32();
        usbWakeupHost(&USB_DRIVER);
    }
}
#endif

#ifdef RGB_MATRIX_ENABLE
void matrix_scan_user(void) {
    // One event per elapsed wait, so the matrix keeps scanning and playback stays
    // interruptible.
    if (play_depth > 0) {
        lpm_timer_reset();
        if (timer_elapsed32(play_timer) >= play_wait) {
            play_done_ms += play_wait;
            play_frame_t *fr = &play_stack[play_depth - 1];
            uint8_t s = fr->slot;
            if (fr->pos >= slot_len(s)) {          // frame finished -> pop
                play_depth--;
                if (play_depth == 0) {
                    if (play_loop) {
                        play_stack[0].slot = play_root;
                        play_stack[0].pos  = 0;
                        play_depth         = 1;
                        // The last event's wait already ran: start over on the next
                        // scan, so a looped tap is down one gap and up one gap.
                        play_wait          = 0;
                        play_done_ms       = 0;
                    } else {
                        stop_playback();
                    }
                } else {
                    // A call's own pause is spent here, after its macro ran.
                    play_frame_t  *pf   = &play_stack[play_depth - 1];
                    macro_event_t *call = slot_ev(pf->slot, pf->pos - 1);
                    play_wait = slot_realdelay(pf->slot) ? call->delay_ms : slot_gap(pf->slot);
                }
            } else {
                macro_event_t *e = slot_ev(s, fr->pos);
                fr->pos++;
                bool called = false;
                if (e->flags & EVF_CALL) {
                    uint8_t target = (uint8_t)e->keycode;
                    if (play_depth < MACRO_PLAY_STACK_DEPTH && call_runs(target)) {
                        play_stack[play_depth].slot = target;
                        play_stack[play_depth].pos  = 0;
                        play_depth++;
                        called = true;             // its delay is spent on the way back
                    }                              // else: depth cap / empty / recording -> skip
                } else if (e->flags & EVF_PRESSED) {
                    register_code16(play_keycode(e->keycode));
                    bool found = false;
                    for (uint8_t h = 0; h < play_nheld; h++) {
                        if (play_held[h] == e->keycode) { found = true; break; }
                    }
                    if (!found) {
                        if (play_nheld < MACRO_PLAY_HELD_MAX) play_held[play_nheld++] = e->keycode;
                        else                                  play_held_overflow = true;
                    }
                } else {
                    unregister_code16(play_keycode(e->keycode));
                    for (uint8_t h = 0; h < play_nheld; h++) {
                        if (play_held[h] == e->keycode) { play_held[h] = play_held[--play_nheld]; break; }
                    }
                }
                play_wait = (called || !slot_realdelay(s)) ? slot_gap(s) : e->delay_ms;
            }
            play_timer = timer_read32();
        }
    }

    static uint32_t repeat_timer = 0;
    // "Repeat last key" only when the loop key isn't driving a macro.
    if (repeat_loop_active && last_macro_slot < 0) {
        lpm_timer_reset();
        if (timer_elapsed32(repeat_timer) >= 50) {
            repeat_timer = timer_read32();
            keyevent_t press = MAKE_KEYEVENT(0, 0, true);
            repeat_key_invoke(&press);
            keyevent_t release = MAKE_KEYEVENT(0, 0, false);
            repeat_key_invoke(&release);
        }
    } else {
        repeat_timer = timer_read32();
    }

    // Keep the board awake while a key is held, so sleep doesn't drop the held report.
    if (hold_loop_active) lpm_timer_reset();

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
            rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_GRADIENT_X);
        } else {
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE);
        }
    }
}

// LED indices: slot keys by global slot index (Q..P = RAM, A..Ö = EEPROM), and the
// two 10-key bars (number row 1..0, bottom letter row Y..-).
#define MACRO_TAB_LED   14
#define MACRO_ENTER_LED 27
#define MACRO_BSPC_LED  13
#define MACRO_CAPS_LED  29
static const uint8_t macro_slot_led[MACRO_TOTAL_SLOTS] = {
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
};
static const uint8_t macro_bar_top[10]    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const uint8_t macro_bar_bottom[10] = {45, 46, 47, 48, 49, 50, 51, 52, 53, 54};

#define MACRO_C_ACTIVE   0x00, 0xFF, 0x00   // green   — executing slot / active loop toggle
#define MACRO_C_PARENT   0xFF, 0x88, 0x00   // orange  — caller still on the play stack
#define MACRO_C_RECORD   0xFF, 0x00, 0x00   // red     — slot being recorded into
#define MACRO_C_ARMED_RD 0xFF, 0x00, 0xFF   // magenta — armed, real-delay mode
#define MACRO_C_ARMED_ND 0xFF, 0x88, 0x00   // orange  — armed, no-delay mode
#define MACRO_C_CFG      0x00, 0x00, 0xFF   // blue    — configure mode (Tab)
#define MACRO_C_CFG_SEL  0xFF, 0xFF, 0xFF   // white   — selected slot (copy source)
#define MACRO_C_EMPTY    0x00, 0xFF, 0x00   // green   — selectable empty slot
#define MACRO_C_USED     0xFF, 0x88, 0x00   // orange  — selectable used slot
#define MACRO_C_OFF      0xFF, 0x00, 0x00   // red     — setting off (Enter: no repeat, Bksp: no trigger)
#define MACRO_C_ON       0x00, 0xFF, 0x00   // green   — toggle repeat / trigger set / key trigger keys
#define MACRO_C_HOLD     0xFF, 0x88, 0x00   // orange  — hold repeat
#define MACRO_C_CAPTURE  0xFF, 0x00, 0xFF   // magenta — capturing a trigger (Bksp)
#define MACRO_C_CHORD    0xFF, 0x00, 0xFF   // magenta — captured chord keys
#define MACRO_C_INVALID  0xFF, 0x00, 0x00   // red     — captured keys that can't be a trigger
#define MACRO_C_PROGRESS 0xFF, 0xFF, 0xFF   // white   — playback progress
#define MACRO_C_RAM_CAP  0x00, 0xFF, 0xFF   // cyan    — RAM pool fill
#define MACRO_C_GAP      0xFF, 0xFF, 0xFF   // white   — selected slot's no-delay gap level
#define MACRO_C_EE_CAP   0xFF, 0x00, 0xFF   // magenta — EEPROM pool fill

// Paint one LED, but only if it falls in the current render window. NO_LED = skip.
static void macro_led(uint8_t lo, uint8_t hi, uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
    if (idx != NO_LED && lo <= idx && idx < hi) rgb_matrix_set_color(idx, r, g, b);
}

// num/den as a 10-LED bar: full LEDs lit, the partial one dimmed to its share, rest off.
static void macro_bar(uint8_t lo, uint8_t hi, const uint8_t leds[10], uint32_t num, uint32_t den,
                      uint8_t r, uint8_t g, uint8_t b) {
    uint32_t fill = den ? (uint32_t)MIN((uint64_t)num * 10 * 255 / den, 10 * 255) : 0;
    for (uint8_t i = 0; i < 10; i++) {
        uint8_t v = fill > 255 ? 255 : (uint8_t)fill;
        fill -= v;
        macro_led(lo, hi, leds[i], r * v / 255, g * v / 255, b * v / 255);
    }
}

static uint32_t play_elapsed_ms(void) {
    return play_done_ms + MIN(timer_elapsed32(play_timer), (uint32_t)play_wait);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Loop-style toggles: solid green while active.
    macro_led(led_min, led_max, rctrl_loop_active  ? 57 : NO_LED, MACRO_C_ACTIVE);
    macro_led(led_min, led_max, repeat_loop_active ? 27 : NO_LED, MACRO_C_ACTIVE);
    macro_led(led_min, led_max, hold_loop_active   ? 13 : NO_LED, MACRO_C_ACTIVE);
    macro_led(led_min, led_max, is_combo_enabled() ? NO_LED : MACRO_CAPS_LED, MACRO_C_OFF);  // chords off

    // Trigger capture: only the captured keys, colored by what they would become.
    if (macro_state == ST_CFG_CAPTURE) {
        macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_CFG);
        macro_led(led_min, led_max, MACRO_BSPC_LED, MACRO_C_CAPTURE);
        uint8_t kind = cap_kind();
        for (uint8_t i = 0; i < cap.npos; i++) {
            uint8_t led = g_led_config.matrix_co[cap.pos[i].row][cap.pos[i].col];
            if (kind == TRIG_KEY)        macro_led(led_min, led_max, led, MACRO_C_ON);
            else if (kind == TRIG_CHORD) macro_led(led_min, led_max, led, MACRO_C_CHORD);
            else                         macro_led(led_min, led_max, led, MACRO_C_INVALID);
        }
    }

    // Slot selection (armed / configure): slot fill state, plus both pools' capacity bars.
    // Armed, the EEPROM slots stay dark: they can't be recorded into.
    bool armed = macro_state == ST_ARMED;
    bool cfg   = macro_state == ST_CFG || macro_state == ST_CFG_SEL;
    if (armed || cfg) {
        if (armed && armed_realdelay) macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_RD);
        else if (armed)               macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_ND);
        else                          macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_CFG);
        for (uint8_t s = 0; s < MACRO_TOTAL_SLOTS; s++) {
            uint8_t led = macro_slot_led[s];
            if (macro_state == ST_CFG_SEL && s == cfg_slot) macro_led(led_min, led_max, led, MACRO_C_CFG_SEL);
            else if (armed && slot_is_ee(s))                continue;
            else if (slot_len(s) > 0)                       macro_led(led_min, led_max, led, MACRO_C_USED);
            else                                            macro_led(led_min, led_max, led, MACRO_C_EMPTY);
        }
        if (macro_state != ST_CFG_SEL) {   // with a slot selected the number row shows its gap
            macro_bar(led_min, led_max, macro_bar_top, ram_hdr.used, MACRO_RAM_EVENTS, MACRO_C_RAM_CAP);
        }
        macro_bar(led_min, led_max, macro_bar_bottom, ee_img.hdr.used, MACRO_EE_EVENTS, MACRO_C_EE_CAP);
    }

    // Selected slot's settings (read-only for EEPROM slots): Enter = repeat mode,
    // Bksp = alternative trigger set or not.
    if (macro_state == ST_CFG_SEL) {
        const macro_cfg_t *c   = slot_cfg(cfg_slot);
        uint8_t            rep = CFG_REPEAT(c);
        if (rep == REP_TOGGLE)    macro_led(led_min, led_max, MACRO_ENTER_LED, MACRO_C_ON);
        else if (rep == REP_HOLD) macro_led(led_min, led_max, MACRO_ENTER_LED, MACRO_C_HOLD);
        else                      macro_led(led_min, led_max, MACRO_ENTER_LED, MACRO_C_OFF);
        if (CFG_TRIG(c) != TRIG_NONE) macro_led(led_min, led_max, MACRO_BSPC_LED, MACRO_C_ON);
        else                          macro_led(led_min, led_max, MACRO_BSPC_LED, MACRO_C_OFF);
        // Gap level n lights keys 1..n+1; real-delay slots have no gap, the row stays dark.
        if (!slot_realdelay(cfg_slot)) {
            macro_bar(led_min, led_max, macro_bar_top, CFG_GAP(c) + 1, 10, MACRO_C_GAP);
        }
    }

    // Recording: the slot currently being written glows red; the number row shows how
    // full the RAM pool is (unless a composed call is playing, see below).
    if (macro_state == ST_RECORDING) {
        macro_led(led_min, led_max, macro_slot_led[active_slot], MACRO_C_RECORD);
        if (play_depth == 0) {
            macro_bar(led_min, led_max, macro_bar_top, ram_hdr.used, MACRO_RAM_EVENTS, MACRO_C_RAM_CAP);
        }
    }

    // Playing: progress on the number row (only for passes of MACRO_FINISH_MS and up,
    // shorter ones would just flicker); the call stack — callers orange, the
    // executing (top) slot green. Green is written last so it wins when a slot appears
    // at multiple depths.
    if (play_depth > 0) {
        if (play_total_ms >= MACRO_FINISH_MS) {
            macro_bar(led_min, led_max, macro_bar_top, play_elapsed_ms(), play_total_ms, MACRO_C_PROGRESS);
        }
        for (uint8_t i = 0; i + 1 < play_depth; i++) {
            macro_led(led_min, led_max, macro_slot_led[play_stack[i].slot], MACRO_C_PARENT);
        }
        macro_led(led_min, led_max, macro_slot_led[play_stack[play_depth - 1].slot], MACRO_C_ACTIVE);
    }
    return false;
}
#endif
