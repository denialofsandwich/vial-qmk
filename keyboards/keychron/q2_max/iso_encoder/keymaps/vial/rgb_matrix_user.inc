// Green (#00ff44, hue 96) to blue (#0044ff, hue 159) gradient across the board,
// computed from each LED's x position so it needs no per-key color table.
RGB_MATRIX_EFFECT(GRADIENT_X)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool GRADIENT_X(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        // x spans 0..224
        hsv_t hsv = {96 + (g_led_config.point[i].x * 63 + 112) / 224, 255, rgb_matrix_config.hsv.v};
        rgb_t rgb = hsv_to_rgb(hsv);
        rgb_matrix_region_set_color(params->region, i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
