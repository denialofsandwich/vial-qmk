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
        _______,  PSLOT_1,  PSLOT_2,  PSLOT_3,  PSLOT_4,  PSLOT_5,  PSLOT_6,  PSLOT_7,  PSLOT_8,  PSLOT_9,  PSLOT_10, MS_BTN5,  MS_BTN2, REPEAT_LOOP,            MS_WHLD,
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
//             triple tap -> copy mode; recording -> stop and save (no keys = erase)
//             also the escape hatch out of any state
// FN2+Q..P  : armed -> record into that RAM slot | idle -> play it
// FN2+A..Ö  : idle -> play that EEPROM slot (it can't be recorded into, only copied to)
// any slot  : recording -> play it live AND record a call to it (compose macros)
//             copy mode -> 1st press picks the source, 2nd the destination; copying
//             an empty slot clears the destination. This is how EEPROM slots are filled
//             playing -> interrupt
// FN2+Enter : loop the last played slot, else "repeat last key". Ignored while
//             armed or recording (a loop records nothing and, since a call is only
//             recorded when nothing plays, would swallow every slot press).
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

typedef struct __attribute__((packed)) {
    uint16_t off[MACRO_SLOT_COUNT];  // first event in the pool (only valid when len > 0)
    uint16_t len[MACRO_SLOT_COUNT];
    uint16_t realdelay;              // bitmask: playback honors real delays?
    uint16_t used;                   // events in use, all below this index
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

typedef enum { ST_IDLE, ST_ARMED, ST_RECORDING, ST_COPY_SRC, ST_COPY_DST } macro_state_t;

static macro_state_t  macro_state     = ST_IDLE;
static bool           armed_realdelay = false;
static uint8_t        active_slot     = 0;               // RAM slot currently being recorded into
static uint8_t        copy_src        = 0;               // copy mode: chosen source slot
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
}

// Copy src's events to the tail of dst's bank (an empty src clears dst). False = no
// room; dst is left untouched then.
static bool copy_slot(uint8_t src, uint8_t dst) {
    if (src == dst) return true;
    const macro_bank_t *db = slot_bank(dst);
    uint8_t             dl = slot_local(dst);
    uint16_t            n  = slot_len(src);
    if ((uint32_t)db->hdr->used - db->hdr->len[dl] + n > db->cap) return false;
    bool rd = slot_realdelay(src);
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
    if (slot_is_ee(dst)) ee_save();
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
    if (ram_hdr.len[s] == 0) ram_hdr.off[s] = 0;  // nothing typed -> slot erased
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
// a key waits w (real delay or MACRO_NODELAY_MS); a call waits MACRO_NODELAY_MS, runs
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
        uint16_t       w = rd ? e->delay_ms : MACRO_NODELAY_MS;
        if ((e->flags & EVF_CALL) && call_runs((uint8_t)e->keycode)) {
            total += MACRO_NODELAY_MS + slot_duration((uint8_t)e->keycode);
        }
        total += w;
    }
    dur_memo[s]  = total;
    dur_state[s] = 2;
    return total;
}

// Not clear_keyboard(): that would also drop keys and mods the user physically holds
// while a called macro ends (and wipe mousekeys).
static void release_play_held(void) {
    if (play_held_overflow) clear_keyboard();            // fallback: never leave a key stuck
    else while (play_nheld) unregister_code16(play_held[--play_nheld]);
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

static void stop_hold_loop(void) {
    if (hold_keycode != KC_NO) {
        unregister_code16(hold_keycode);
        unregister_mods(hold_mods);
    }
    hold_keycode     = KC_NO;
    hold_mods        = 0;
    hold_loop_active = false;
}

void keyboard_post_init_user(void) {
    ee_load();
}

// EEPROM reset (also on first boot): the datablock was just zeroed, drop the mirror too.
void eeconfig_init_user(void) {
    memset(&ee_img, 0, sizeof(ee_img));
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int8_t slot = slot_index(keycode);

    if (keycode == DREC) {
        if (record->event.pressed) {
            stop_hold_loop();
            if (play_depth > 0) stop_playback();
            // The "repeat last key" loop runs with play_depth == 0 and re-enters
            // process_record_user, so leaving it on would flood a fresh recording.
            repeat_loop_active = false;
            if (macro_state == ST_RECORDING) {
                stop_recording();
            } else if (macro_state == ST_ARMED) {
                if (timer_elapsed32(drec_tap_timer) >= DREC_TAP_TERM) {
                    macro_state = ST_IDLE;         // settled press while armed -> abort
                } else if (!armed_realdelay) {
                    armed_realdelay = true;        // quick double tap -> measure real delays
                } else {
                    macro_state = ST_COPY_SRC;     // quick triple tap -> copy mode
                }
            } else if (macro_state == ST_COPY_SRC || macro_state == ST_COPY_DST) {
                macro_state = ST_IDLE;             // abort copy
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
            stop_hold_loop();
            if (macro_state == ST_RECORDING) {
                // Run the slot live *and* record a call to it. last_macro_slot stays
                // put, so a later FN2+Enter loops the macro being built.
                if (play_depth == 0 && call_runs((uint8_t)slot)) {
                    record_call((uint8_t)slot);            // first: it stamps last_event_time
                    start_playback((uint8_t)slot, false);
                }
            } else if (play_depth > 0) {           // press during playback -> interrupt
                stop_playback();
                repeat_loop_active = false;
            } else if (macro_state == ST_ARMED) {
                if (!slot_is_ee((uint8_t)slot)) start_recording((uint8_t)slot);  // EEPROM: copy only
            } else if (macro_state == ST_COPY_SRC) {
                copy_src    = (uint8_t)slot;
                macro_state = ST_COPY_DST;
            } else if (macro_state == ST_COPY_DST) {
                copy_slot(copy_src, (uint8_t)slot);
                macro_state = ST_IDLE;
            } else if (slot_len((uint8_t)slot) > 0) {  // ST_IDLE
                last_macro_slot = slot;
                start_playback((uint8_t)slot, false);
            }
        }
        return false;
    }

    // A normal key press becomes the new "last action" for FN2+Enter. is_recordable()
    // excludes layer keys, so holding FN2 to reach Enter doesn't reset this.
    if (record->event.pressed && is_recordable(keycode, record)) {
        last_macro_slot = -1;
    }

    if (macro_state == ST_RECORDING && is_recordable(keycode, record)) {
        record_event(keycode, record->event.pressed);
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
        case RCTRL_LOOP:
            if (record->event.pressed) {
                rctrl_loop_active = !rctrl_loop_active;
            }
            return false;
        case REPEAT_LOOP:
            if (record->event.pressed) {
                if (macro_state != ST_IDLE) return false;  // see the header comment
                stop_hold_loop();  // mutually exclusive with the hold loop
                repeat_loop_active = !repeat_loop_active;
                if (repeat_loop_active) {
                    if (last_macro_slot >= 0 && slot_len((uint8_t)last_macro_slot) > 0) {
                        start_playback((uint8_t)last_macro_slot, true);
                    }
                    // else: "repeat last key" handled in matrix_scan_user
                } else {
                    stop_playback();
                }
            }
            return false;
        case HOLD_LOOP:
            if (record->event.pressed) {
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
                        play_wait          = MACRO_NODELAY_MS;
                        play_done_ms       = 0;
                    } else {
                        stop_playback();
                    }
                } else {
                    // A call's own pause is spent here, after its macro ran.
                    play_frame_t  *pf   = &play_stack[play_depth - 1];
                    macro_event_t *call = slot_ev(pf->slot, pf->pos - 1);
                    play_wait = slot_realdelay(pf->slot) ? call->delay_ms : MACRO_NODELAY_MS;
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
                    register_code16(e->keycode);
                    bool found = false;
                    for (uint8_t h = 0; h < play_nheld; h++) {
                        if (play_held[h] == e->keycode) { found = true; break; }
                    }
                    if (!found) {
                        if (play_nheld < MACRO_PLAY_HELD_MAX) play_held[play_nheld++] = e->keycode;
                        else                                  play_held_overflow = true;
                    }
                } else {
                    unregister_code16(e->keycode);
                    for (uint8_t h = 0; h < play_nheld; h++) {
                        if (play_held[h] == e->keycode) { play_held[h] = play_held[--play_nheld]; break; }
                    }
                }
                play_wait = (called || !slot_realdelay(s)) ? MACRO_NODELAY_MS : e->delay_ms;
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
#define MACRO_TAB_LED 14
static const uint8_t macro_slot_led[MACRO_TOTAL_SLOTS] = {
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
};
static const uint8_t macro_bar_top[10]    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const uint8_t macro_bar_bottom[10] = {45, 46, 47, 48, 49, 50, 51, 52, 53, 54};

#define MACRO_C_ACTIVE   0x00, 0xFF, 0x00   // green   — executing slot / active loop toggle
#define MACRO_C_PARENT   0xFF, 0xFF, 0x00   // yellow  — caller still on the play stack
#define MACRO_C_RECORD   0xFF, 0x00, 0x00   // red     — slot being recorded into
#define MACRO_C_ARMED_RD 0xFF, 0x00, 0xFF   // magenta — armed, real-delay mode
#define MACRO_C_ARMED_ND 0xFF, 0xFF, 0x00   // yellow  — armed, no-delay mode
#define MACRO_C_COPY     0x00, 0x00, 0xFF   // blue    — copy mode (Tab)
#define MACRO_C_COPY_SRC 0xFF, 0xFF, 0xFF   // white   — chosen copy source
#define MACRO_C_EMPTY    0x00, 0xFF, 0x00   // green   — selectable empty slot
#define MACRO_C_USED     0xFF, 0xFF, 0x00   // yellow  — selectable used slot
#define MACRO_C_LOCKED   0xFF, 0x00, 0x00   // red     — not selectable (EEPROM while armed)
#define MACRO_C_PROGRESS 0xFF, 0xFF, 0xFF   // white   — playback progress
#define MACRO_C_RAM_CAP  0x00, 0xFF, 0xFF   // cyan    — RAM pool fill
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

    // Slot selection (armed / copy): slot fill state, plus both pools' capacity bars.
    bool armed   = macro_state == ST_ARMED;
    bool copying = macro_state == ST_COPY_SRC || macro_state == ST_COPY_DST;
    if (armed || copying) {
        if (armed && armed_realdelay) macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_RD);
        else if (armed)               macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_ARMED_ND);
        else                          macro_led(led_min, led_max, MACRO_TAB_LED, MACRO_C_COPY);
        for (uint8_t s = 0; s < MACRO_TOTAL_SLOTS; s++) {
            uint8_t led = macro_slot_led[s];
            if (macro_state == ST_COPY_DST && s == copy_src) macro_led(led_min, led_max, led, MACRO_C_COPY_SRC);
            else if (armed && slot_is_ee(s))                 macro_led(led_min, led_max, led, MACRO_C_LOCKED);
            else if (slot_len(s) > 0)                        macro_led(led_min, led_max, led, MACRO_C_USED);
            else                                             macro_led(led_min, led_max, led, MACRO_C_EMPTY);
        }
        macro_bar(led_min, led_max, macro_bar_top, ram_hdr.used, MACRO_RAM_EVENTS, MACRO_C_RAM_CAP);
        macro_bar(led_min, led_max, macro_bar_bottom, ee_img.hdr.used, MACRO_EE_EVENTS, MACRO_C_EE_CAP);
    }

    // Recording: the slot currently being written glows red; the number row shows how
    // full the RAM pool is (unless a composed call is playing, see below).
    if (macro_state == ST_RECORDING) {
        macro_led(led_min, led_max, macro_slot_led[active_slot], MACRO_C_RECORD);
        if (play_depth == 0) {
            macro_bar(led_min, led_max, macro_bar_top, ram_hdr.used, MACRO_RAM_EVENTS, MACRO_C_RAM_CAP);
        }
    }

    // Playing: progress on the number row; the call stack — callers yellow, the
    // executing (top) slot green. Green is written last so it wins when a slot appears
    // at multiple depths.
    if (play_depth > 0) {
        macro_bar(led_min, led_max, macro_bar_top, play_elapsed_ms(), play_total_ms, MACRO_C_PROGRESS);
        for (uint8_t i = 0; i + 1 < play_depth; i++) {
            macro_led(led_min, led_max, macro_slot_led[play_stack[i].slot], MACRO_C_PARENT);
        }
        macro_led(led_min, led_max, macro_slot_led[play_stack[play_depth - 1].slot], MACRO_C_ACTIVE);
    }
    return false;
}
#endif
