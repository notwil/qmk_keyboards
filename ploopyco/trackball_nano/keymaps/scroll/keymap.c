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
#include "math.h"
#include QMK_KEYBOARD_H

#define ACCEL_FACTOR 8
#define SCROLL_X_THRESHOLD 60
#define SCROLL_Y_THRESHOLD 30
#define TRANSLATE_ANGLE 0
#define PRECISIONMODE_FACTOR 0.6
// safe range starts at `PLOOPY_SAFE_RANGE` instead.
int16_t delta_x = 0;
int16_t delta_y = 0;

void process_mouse_user(report_mouse_t *mouse_report, float x, float y) {

	//# If scroll lock is registered, start the scrolling action
	// https://www.reddit.com/r/ploopy/comments/nlvgkq/how_to_scroll_with_the_trackball_nano/
	if (host_keyboard_led_state().scroll_lock) {

		delta_x += x;
		delta_y += y;
		
		if (delta_x > SCROLL_X_THRESHOLD) {
			mouse_report->h = 1;
			mouse_report->v = 0;
			delta_x = 0;
		} else if (delta_x < -SCROLL_X_THRESHOLD) {
			mouse_report->h = -1;
			mouse_report->v = 0;
			delta_x = 0;
		}
		else if (delta_y > SCROLL_Y_THRESHOLD) {
			mouse_report->v = -1;
			mouse_report->h = 0;
			delta_y = 0;
		} else if (delta_y < -SCROLL_Y_THRESHOLD) {
			mouse_report->v = 1;
			mouse_report->h = 0;
			delta_y = 0;
		}
	} 
	//# otherwise the ploopy performs as a mouse
	else {
		mouse_report->x = (int16_t) x;
		mouse_report->y = (int16_t) y;
	}
}


double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}