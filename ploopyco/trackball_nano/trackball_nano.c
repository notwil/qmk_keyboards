/* Copyright 2021 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2019 Hiroyuki Okada
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "trackball_nano.h"
#include "wait.h"

#ifndef OPT_DEBOUNCE
#    define OPT_DEBOUNCE 5  // (ms) 			Time between scroll events
#endif

#ifndef SCROLL_BUTT_DEBOUNCE
#    define SCROLL_BUTT_DEBOUNCE 100  // (ms) 			Time between scroll events
#endif

#ifndef OPT_THRES
#    define OPT_THRES 150  // (0-1024) 	Threshold for actication
#endif

#ifndef OPT_SCALE
#    define OPT_SCALE 1  // Multiplier for wheel
#endif

#ifndef PLOOPY_DPI_OPTIONS
#    define PLOOPY_DPI_OPTIONS \
        { 375, 750, 1375 }
#    ifndef PLOOPY_DPI_DEFAULT
#        define PLOOPY_DPI_DEFAULT 2
#    endif
#endif

#ifndef PLOOPY_DPI_DEFAULT
#    define PLOOPY_DPI_DEFAULT 2
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = { };
const static float ADNS_X_TRANSFORM = -1.0;
const static float ADNS_Y_TRANSFORM = 1.0;

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
#define DPI_OPTION_SIZE (sizeof(dpi_array) / sizeof(uint16_t))

bool is_scroll_clicked = false;
bool BurstState = false;  // init burst state for Trackball module
uint16_t MotionStart = 0;  // Timer for accel, 0 is resting state
uint16_t lastScroll = 0;  // Previous confirmed wheel event
uint16_t lastMidClick = 0;  // Stops scrollwheel from being read if it was pressed
bool debug_encoder = false;

void pointing_device_init_kb(void) {
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
}

__attribute__((weak)) void process_mouse_user(report_mouse_t* mouse_report, float x, float y) {
    mouse_report->x = x;
    mouse_report->y = y;
}

__attribute__((weak)) void process_mouse(report_mouse_t* mouse_report) {
    report_adns5050_t data = adns5050_read_burst();
    if (data.dx != 0 || data.dy != 0) {
        if (debug_mouse)
            dprintf("Raw ] X: %d, Y: %d\n", data.dx, data.dy);
        float xt = (float) data.dy * ADNS_X_TRANSFORM;
        float yt = (float) data.dx * ADNS_Y_TRANSFORM;
        process_mouse_user(mouse_report, xt, yt);
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    xprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
    // Update Timer to prevent accidental scrolls
    if ((record->event.key.col == 1) && (record->event.key.row == 0)) {
        lastMidClick = timer_read();
        is_scroll_clicked = record->event.pressed;
    }
    if (!process_record_user(keycode, record))
        return false;
    return true;
}

void keyboard_pre_init_kb(void) {
    debug_enable = true;
    debug_keyboard = true;
    debug_mouse = true;
    // debug_matrix = true;
    // debug_mouse = true;
    // debug_encoder = true;

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
#ifdef UNUSED_PINS
    const pin_t unused_pins[] = UNUSED_PINS;
    for (uint8_t i = 0; i < (sizeof(unused_pins) / sizeof(pin_t)); i++) {
        setPinOutput(unused_pins[i]);
        writePinLow(unused_pins[i]);
    }
#endif

    keyboard_pre_init_user();
}

void pointing_device_init(void) {
    adns5050_init();
    wait_ms(55);
    adns5050_read_burst();
    adns5050_set_cpi(dpi_array[keyboard_config.dpi_config]);
}

void pointing_device_task(void) {
    report_mouse_t mouse_report = pointing_device_get_report();
    process_mouse(&mouse_report);
    pointing_device_set_report(mouse_report);
    pointing_device_send();
}

void eeconfig_init_kb(void) {
    keyboard_config.dpi_config = PLOOPY_DPI_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    matrix_init_user();
}