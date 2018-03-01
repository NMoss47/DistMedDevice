/*
 * Copyright 2017 Nicholas Moss
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy * of this software and associated documentation files (the "Software"), to    * deal in the Software without restriction, including without limitation the  * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or * sell copies of the Software, and to permit persons to whom the Software is   * furnishedto do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in   * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN   * THE SOFTWARE.
*/

/*
 * Senior Project - Team A
 * Distributed Medical Devices
 * Nick Moss, Aaron Neff, Matthew Smith
*/

/*
 * Main Application Code
 * Author: Nicholas Moss
 * 
*/

/*******************************************************************************
*   Includes
*******************************************************************************/

#include "include/distmed_adc.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

void pulseoxSignalSet( void )
{
    // GPIO pin configuration structures
    gpio_config_t redled;
    gpio_config_t infraredled;
    
    // Red LED configuration
    redled.intr_type = GPIO_INTR_DISABLE;
    redled.mode = GPIO_MODE_OUTPUT;
    redled.pin_bit_mask = RED_GPIO;
    redled.pull_down_en = 1;
    redled.pull_up_en = 0;
    gpio_config(&redled);
    
    // Infrared LED configuration
    infraredled.intr_type = GPIO_INTR_DISABLE;
    infraredled.mode = GPIO_MODE_OUTPUT;
    infraredled.pin_bit_mask = INFRARED_GPIO;
    infraredled.pull_down_en = 1;
    infraredled.pull_up_en = 0;
    gpio_config(&infraredled);
    
    // This loop sets the pulsing frequencies of the two LEDs
    while(true)
    {
        gpio_set_level(RED_GPIO, 1);
        gpio_set_level(INFRARED_GPIO, 0);
        vTaskDelay(DELAYVAL);
        gpio_set_level(RED_GPIO, 0);
        gpio_set_level(INFRARED_GPIO, 1);
        vTaskDelay(DELAYVAL);
    }
}

void readADC( void )
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_0db);
    
    // Calculate ADC characteristics
    esp_adc_cal_characteristics_t characteristics;
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_0db, ADC_WIDTH_12Bit, &characteristics);
    
    while(1)
    {
        uint32_t voltage = adc1_to_voltage(ADC1_CHANNEL_7, &characteristics);
        uint32_t reading = adc1_get_raw(ADC1_CHANNEL_7);
        printf("%d mV | %d MV\n", voltage, reading);
        vTaskDelay(500/portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}
