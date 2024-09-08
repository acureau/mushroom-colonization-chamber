#include <stdio.h>
#include "pico/stdlib.h"
#include "dht22-pico.h"

// Pin Mapping
#define TEMP_HUMIDITY_SENSOR_PIN 0
#define FANS_SWITCH_PIN 19
#define HEAT_PADS_SWITCH_PIN 18

// Controller Parameters
#define MS_BETWEEN_CONTROLLER_CYCLES 60000
#define MINIMUM_TEMPERATURE 70.0
#define MAXIMUM_TEMPERATURE 80.0
#define AIR_CYCLE_MINS_PER_HOUR 15.0

// The temperature / humidity sensor object.
dht_reading temp_humidity_sensor;
uint64_t last_air_cycle_time;

// Initializes all SDK and hardware components.
void initialize() 
{
    // Delay one cycle to ensure proper initialization.
    sleep_ms(MS_BETWEEN_CONTROLLER_CYCLES);

    // Configure USB stdio.
    stdio_usb_init();

    // Configure built-in LED GPIO settings, set on to indicate powered.
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);

    // Configure fans switch GPIO settings.
    gpio_init(FANS_SWITCH_PIN);
    gpio_set_dir(FANS_SWITCH_PIN, GPIO_OUT);
    gpio_put(FANS_SWITCH_PIN, false);

    // Configure heat pads switch GPIO settings.
    gpio_init(HEAT_PADS_SWITCH_PIN);
    gpio_set_dir(HEAT_PADS_SWITCH_PIN, GPIO_OUT);
    gpio_put(HEAT_PADS_SWITCH_PIN, false);

    // Initialize temperature sensor.
    temp_humidity_sensor = dht_init(TEMP_HUMIDITY_SENSOR_PIN);

    // Initialize air flow cycle.
    last_air_cycle_time = time_us_64();
}

// Reads the temperature from the sensor in farenheit.
float read_temperature()
{
    return (float)((temp_humidity_sensor.temp_celsius * 9.0 / 5.0) + 32.0);
}

// Determines if the air cycle time is active.
bool is_air_cycle_active()
{
    uint64_t current_time = time_us_64();
    float minutes_since_last_air_cycle = (float)((current_time - last_air_cycle_time) / 60000000.0f);
    if (minutes_since_last_air_cycle < AIR_CYCLE_MINS_PER_HOUR)
    {
        return true;
    }
    else if (minutes_since_last_air_cycle >= 60.0f)
    {
        last_air_cycle_time = current_time;
    }
    return false;
}

int main() 
{
    // Initialize the system. 
    initialize();

    // Main controller cycle.
    while (true) 
    {
        // Read temperature from sensor.
        if (dht_read(&temp_humidity_sensor) == DHT_OK) 
        {
            // Get temperature in farenheit.
            float temperature = read_temperature();

            // Turn on fans when above maximum allowed temperature, or when air cycle is active.
            bool air_cycle_active = (temperature > MAXIMUM_TEMPERATURE || is_air_cycle_active());

            // Turn on heater when below minimum allowed temperature.
            bool heat_pads_active = (temperature < MINIMUM_TEMPERATURE);

            // Set GPIO and debut output.
            gpio_put(FANS_SWITCH_PIN, air_cycle_active);
            gpio_put(HEAT_PADS_SWITCH_PIN, heat_pads_active);
            printf("Current Temperature: %.2fF, Air Cycle Active: %s, Heat Pads Active: %s\n", temperature, air_cycle_active ? "Yes" : "No", heat_pads_active ? "Yes" : "No");
        }

        // If sensor reading fails turn off light and halt execution.
        else 
        {
            printf("Temperature sensor reading failed.\n");
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            break;
        }

        // Wait for next cycle.
        sleep_ms(MS_BETWEEN_CONTROLLER_CYCLES);
    }
    return 0;
}