/*
    Copyright (c) 2021 Eleanor McMurtry

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * DHT22-Pico v0.2.0 by Eleanor McMurtry (2021)
 * Loosely based on code by Caroline Dunn: https://github.com/carolinedunn/pico-weather-station
 **/

#ifndef DHT22_PICO_H
#define DHT22_PICO_H

#include "pico/stdlib.h"

#define DHT_OK              0
#define DHT_ERR_CHECKSUM    1
#define DHT_ERR_NAN         2

typedef struct {
    uint pin;
    float humidity;
    float temp_celsius;
} dht_reading;

dht_reading dht_init(uint8_t pin);

uint8_t dht_read(dht_reading *dht);

#endif