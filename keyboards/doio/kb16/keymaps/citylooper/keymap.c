/* City Looper - Minimal Build Test Keymap
 * DOIO KB16 (rev2)
 *
 * This is a placeholder keymap for build verification only.
 * No functional logic implemented yet.
 */

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Minimal placeholder layout for build test
     *
     *     ┌───┬───┬───┬───┐   ┌───┐ ┌───┐
     *     │ 1 │ 2 │ 3 │ 4 │   │ A │ │ B │
     *     ├───┼───┼───┼───┤   └───┘ └───┘
     *     │ 5 │ 6 │ 7 │ 8 │
     *     ├───┼───┼───┼───┤
     *     │ 9 │ 0 │ C │ D │      ┌───┐
     *     ├───┼───┼───┼───┤      │ E │
     *     │ F │ ← │ ↓ │ → │      └───┘
     *     └───┴───┴───┴───┘
     */
    [_BASE] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_A,
        KC_5,    KC_6,    KC_7,    KC_8,    KC_B,
        KC_9,    KC_0,    KC_C,    KC_D,    KC_E,
        KC_F,    KC_LEFT, KC_DOWN, KC_RIGHT
    )
};

