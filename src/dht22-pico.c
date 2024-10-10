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

/*
    DHT22-Pico v0.2.0 by Eleanor McMurtry (2021)
    Loosely based on code by Caroline Dunn: https://github.com/carolinedunn/pico-weather-station
*/

#include "dht22-pico.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <math.h>

dht_reading dht_init(uint8_t pin) {
    gpio_init(pin);
    dht_reading dht;
    dht.pin = pin;
    return dht;
}

static uint32_t wait_for(uint8_t pin, uint8_t expect) {
    uint32_t then = time_us_32();
    while (expect != gpio_get(pin)) {
        sleep_us(10);
    }
    return time_us_32() - then;
}

inline static float word(uint8_t first, uint8_t second) {
    return (float) ((first << 8) + second);
}

// helper function to read from the DHT
// more: https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
uint8_t dht_read(dht_reading *dht) {
    uint8_t data[5] = {0, 0, 0, 0, 0};

    // request a sample
    gpio_set_dir(dht->pin, GPIO_OUT);
    gpio_put(dht->pin, 0);
    sleep_ms(10);
    gpio_put(dht->pin, 1);
    sleep_us(40);
    
    // wait for acknowledgement
    gpio_set_dir(dht->pin, GPIO_IN);
    wait_for(dht->pin, 0);
    wait_for(dht->pin, 1);
    wait_for(dht->pin, 0);

    // read sample (40 bits = 5 bytes)
    for (uint8_t bit = 0; bit < 40; ++bit) {
        wait_for(dht->pin, 1);
        uint8_t count = wait_for(dht->pin, 0);
        data[bit / 8] <<= 1;
        if (count > 50) {
            data[bit / 8] |= 1;
        }
    }

    // pull back up to mark end of read
    gpio_set_dir(dht->pin, GPIO_OUT);
    gpio_put(dht->pin, 1);

    // checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        float humidity = word(data[0], data[1]) / 10;
        float temp = word(data[2] & 0x7F, data[3]) / 10;

        // if the highest bit is 1, temperature is negative
        if (data[2] & 0x80) {
            temp = -temp;
        }

        // check if checksum was OK but something else went wrong
        if (isnan(temp) || isnan(humidity) ||temp == 0) {
            return DHT_ERR_NAN;
        } else {
            dht->humidity = humidity;
            dht->temp_celsius = temp;
            return DHT_OK;
        }
    } else {
        return DHT_ERR_CHECKSUM;
    }
}