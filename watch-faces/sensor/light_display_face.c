/*
 * MIT License
 *
 * Copyright (c) 2026 Sensor Watch contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "light_display_face.h"
#include "watch.h"

// Set to true in setup if this board has no light sensor; the face then skips itself.
static bool skip = false;

#ifdef HAS_IR_SENSOR

// Mirrors the thermistor driver: power the sensor only for the duration of the reading,
// and only enable the ADC / analog pin mux while we're actually sampling.
static uint16_t _light_display_face_read_sensor(void) {
    watch_enable_adc();
    // Route the sense pin to the ADC.
    HAL_GPIO_IRSENSE_in();
    HAL_GPIO_IRSENSE_pmuxen(HAL_GPIO_PMUX_ADC);
    // IR_ENABLE is active low: drive it low to power the phototransistor circuit.
    HAL_GPIO_IR_ENABLE_out();
    HAL_GPIO_IR_ENABLE_clr();

    uint16_t value = watch_get_analog_pin_level(HAL_GPIO_IRSENSE_pin());

    // Power everything back down to save energy.
    HAL_GPIO_IR_ENABLE_set();
    HAL_GPIO_IR_ENABLE_off();
    HAL_GPIO_IRSENSE_pmuxdis();
    HAL_GPIO_IRSENSE_off();
    watch_disable_adc();

    return value;
}

#else

static uint16_t _light_display_face_read_sensor(void) {
    return 0;
}

#endif // HAS_IR_SENSOR

static void _light_display_face_update_display(void) {
    char buf[7];
    uint16_t light_level = _light_display_face_read_sensor();
    snprintf(buf, sizeof(buf), "%6u", light_level);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void light_display_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    (void) context_ptr;
#ifndef HAS_IR_SENSOR
    // no light sensor on this board, which means we shouldn't be here.
    skip = true;
#endif
}

void light_display_face_activate(void *context) {
    (void) context;
}

bool light_display_face_loop(movement_event_t event, void *context) {
    (void) context;
    watch_date_time_t date_time = watch_rtc_get_date_time();
    switch (event.event_type) {
        case EVENT_ACTIVATE:
            if (skip) {
                movement_move_to_next_face();
                return false;
            }
            if (watch_sleep_animation_is_running()) watch_stop_sleep_animation();
            watch_display_text_with_fallback(WATCH_POSITION_TOP, "LIGHT", "LI");
            // force a measurement to be taken immediately.
            date_time.unit.second = 0;
            // fall through
        case EVENT_TICK:
            if (date_time.unit.second % 5 == 4) {
                // Turn the indicator on a second before the reading is taken, and clear it when we're done.
                watch_set_indicator(WATCH_INDICATOR_SIGNAL);
            } else if (date_time.unit.second % 5 == 0) {
                _light_display_face_update_display();
                watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
            }
            break;
        case EVENT_LOW_ENERGY_UPDATE:
            // clear seconds area and start tick animation if necessary
            if (!watch_sleep_animation_is_running()) {
                watch_start_sleep_animation(1000);
            }
            // update every 5 minutes
            if (date_time.unit.minute % 5 == 0) {
                watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
                _light_display_face_update_display();
            }
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
        case EVENT_LIGHT_BUTTON_UP:
        case EVENT_LIGHT_LONG_PRESS:
        case EVENT_LIGHT_LONG_UP:
            // suppress the LED: it sits right next to the sensor and would swamp the reading.
            break;
        default:
            movement_default_loop_handler(event);
            break;
    }

    return true;
}

void light_display_face_resign(void *context) {
    (void) context;
}
