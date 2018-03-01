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

#pragma once
#include "esp_log.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

#define MEAN_FILTER_SIZE        15          //!< Size of the moving avg filter
#define ALPHA                   0.95        //!< Alpha value to remove DC signal

#define PULSE_SAMPLE_SIZE       10          //!< Number of samples for pulse
#define PULSE_MIN_THRESHOLD     -275        //!< Min value of pulse signals
#define PULSE_MAX_THRESHOLD     1000        //!< Max value of pulse signals


/*******************************************************************************
*   Structures
*******************************************************************************/

typedef struct
{
    float w;
    float result;

} dcFilter_t;

typedef struct
{
    float values[MEAN_FILTER_SIZE];
    int index;
    float sum;
    int count;

} meanDiffFilter_t;

typedef struct
{
    float v[2];
    float result;

} butterFilter_t;

typedef struct
{
    float pulse;
    float spo2;

} patientInfo_t;

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This function removed the DC offset of the signal from the MAX30100.
* 
* @param x Input value to be processed.
* @param prev_w Previously processed input value x[n-1]
* @param alpha Alpha value of signal. A value of 1 allows the entire signal to
*   pass, a value of 0 completely block the signal. 0.95 is recommended.
* @return dcfilter_t Structure containing current filtered value and
*   previous filtered value.
*/
dcFilter_t dcRemove( float x, float prev_w, float alpha );

/**
* @brief This function is a basic low pass butterworth filter. Rework since the 
* magic numbers are based off other word. The numbers rerpesent cutoff values.
*
* @param x The input signal sample to be filtered.
* @param filterresult The buffer for the resutling data.
*/
void lpButterFilter( float x, butterFilter_t* filterresult);

/**
* @brief This is a basic moving average filter.
*
* @param m The intput signal sample to be filtered.
* @param filtervalues Struct of the filter to be used.
* @return Float value of the filtered signal sample.
*/
float meanDiff( float m, meanDiffFilter_t* filtervalues );
