#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT(LCTL(KC_PGUP), LCTL(KC_W), LCTL(KC_PGDN),
               KC_BTN4, KC_LSCR, KC_BTN5,
               KC_BTN1, KC_BTN3, KC_BTN2),

  [2] = LAYOUT(KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO),
			  
  [2] = LAYOUT(KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO),
			  
  [3] = LAYOUT(KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO,
               KC_NO, KC_NO, KC_NO),
};
