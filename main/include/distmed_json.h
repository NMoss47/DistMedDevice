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

#include "string.h"
#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"
#include "esp_system.h"
#include "distmed_signalprocessing.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

#define DEVICE_SERIAL_NUMBER    CONFIG_DEVICE_SERIAL_NUMBER
#define DEVICE_NAME             CONFIG_DEVICE_NAME

/*******************************************************************************
*   Structures and Variables
*******************************************************************************/



/*******************************************************************************
*   Function Prototypes
*******************************************************************************/


/**
* @brief This function takes a patientInfo_t stucture and returns a new JSON 
* object.
* 
* @param patientinfo The patientInfo_t stucture that contains strings to be
* inserted into the cJSON object.
* @return cJSON object of the patient information.
*/
cJSON* createRoot( patientInfo_t* patientinfo );

/**
* @brief This function takes an existing cJSON object and a patientInfo_t
* structure and returns an updates cJSON object.
* 
* @param root The cJSON structure to be modified.
* @param patientinfo The patientInfo_t stucture that contains the strings that
* will be inserted into the cJSON object.
* @return Updates cJSON object of the patient information.
*/
cJSON* updateRoot( cJSON* root, patientInfo_t* patientinfo );