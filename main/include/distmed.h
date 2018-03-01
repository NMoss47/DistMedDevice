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

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "esp_log.h"

#include "distmed_networks.h"
#include "distmed_adc.h"
#include "distmed_i2c.h"
#include "distmed_storage.h"
#include "distmed_json.h"

#include "esp_heap_trace.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

/*******************************************************************************
*   Structures and Variables
*******************************************************************************/

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This is the main task of the application. It sets up the i2c interface,
* the socket information, and calls functions to get the pulse and spo2 data.
*/
void mainTask( void );

/**
* @brief The main entry point of the DistMed application. This function
* initializes the system and starts various tasks for the application.
*/
int app_main( void );
