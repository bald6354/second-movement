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

#pragma once

#include "pins.h"

/*
 * LIGHT LEVEL READOUT (aka Light Display)
 *
 * This watch face reads the ambient light level from the phototransistor on
 * the Sensor Watch Pro board (IRSENSE on PA04, powered via IR_ENABLE on PB22)
 * and displays it as a raw 16-bit ADC reading (0-65535). Higher numbers mean
 * more light. The reading is uncalibrated; it is intended as a relative
 * indication of brightness rather than an absolute lux measurement.
 *
 * Like the Temperature Display face, this face samples once every five
 * seconds and illuminates the Signal indicator just before taking a reading.
 * In low energy mode it samples once every five minutes.
 *
 * The LIGHT button does not turn on the LED on this face, since the LED sits
 * right next to the sensor and would swamp the reading.
 *
 * On boards without a light sensor (anything other than Sensor Watch Pro),
 * this face skips itself and moves on to the next face.
 */

#include "movement.h"

void light_display_face_setup(uint8_t watch_face_index, void ** context_ptr);
void light_display_face_activate(void *context);
bool light_display_face_loop(movement_event_t event, void *context);
void light_display_face_resign(void *context);

#define light_display_face ((const watch_face_t){ \
    light_display_face_setup, \
    light_display_face_activate, \
    light_display_face_loop, \
    light_display_face_resign, \
    NULL, \
})
