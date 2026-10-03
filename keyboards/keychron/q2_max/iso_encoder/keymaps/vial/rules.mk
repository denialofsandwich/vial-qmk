ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes
DYNAMIC_MACRO_ENABLE = no
MOUSEKEY_ENABLE = yes
REPEAT_KEY_ENABLE = yes

# VIA/Vial are disabled to free EEPROM: the keymap and encoder map are read from
# keymap.c in flash instead of an EEPROM copy (VIA forces the dynamic keymap on).
VIA_ENABLE = no
VIAL_ENABLE = no
VIALRGB_ENABLE = no

# USB-power gradient effect (rgb_matrix_user.inc), replaces Keychron's per-key RGB
RGB_MATRIX_CUSTOM_USER = yes
