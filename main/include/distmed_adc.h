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

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

#define ADC1_CHAN_4         (4)
#define V_REF               1100
#define RED_GPIO            GPIO_SEL_14
#define INFRARED_GPIO       GPIO_SEL_13
#define DELAYVAL            500

/*******************************************************************************
*   Structures
*******************************************************************************/



/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This function is used to configure the GPIO pins that will control the 
* LED timings for a pulse oximeter.
*/
void pulseoxSignalSet( void );

/**
* @brief This function is used to read the current value of the ADC channel.
*/
void readADC( void );
