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
 * TEST
*/

/*******************************************************************************
*   Includes
*******************************************************************************/

#include "include/distmed_signalprocessing.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

const static char tag[] = "Signal processing";

dcFilter_t dcRemove( float x, float prev_w, float alpha )
{
    dcFilter_t filteredsignal;
    filteredsignal.w = x + alpha * prev_w;
    filteredsignal.result = filteredsignal.w - prev_w;
    
    return filteredsignal;
}


void lpButterFilter( float x, butterFilter_t* filterresult )
{
    filterresult->v[0] = filterresult->v[1];
    // TODO comment why we chose these numbers
    filterresult->v[1] = ( 2.152372752522785600226e-1 * x ) + 
                         ( 0.50952544949442879485 * filterresult->v[0] );
    
    filterresult->result = ( filterresult->v[0] + filterresult->v[1] );
}

float meanDiff( float m, meanDiffFilter_t* filtervalues )
{
    float avg = 0;
    filtervalues->sum -= filtervalues->values[filtervalues->index];
    filtervalues->values[filtervalues->index] = m;
    filtervalues->sum += filtervalues->values[filtervalues->index];
    
    filtervalues->index++;
    filtervalues->index = filtervalues->index % MEAN_FILTER_SIZE;
    
    if( filtervalues->count < MEAN_FILTER_SIZE )
    {
        filtervalues->count++;
    }
    
    avg = filtervalues->sum / filtervalues->count;
    
    return ( avg - m );
}