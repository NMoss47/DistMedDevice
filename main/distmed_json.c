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

#include "include/distmed_json.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

const static char tag[] = "JSON";


cJSON* createRoot( patientInfo_t* patientinfo )
{ 
    cJSON* root = cJSON_CreateObject( );
    
    // Initial information for the device
    
    cJSON_AddStringToObject( root, "deviceID", DEVICE_SERIAL_NUMBER );
    cJSON_AddStringToObject( root, "deviceName", DEVICE_NAME );
    
    // Initial medical information - REWORK
    
	//cJSON_AddNumberToObject( root, "heap", system_get_free_heap_size( ) );
	cJSON_AddNumberToObject( root, "pulse", patientinfo->pulse );
	cJSON_AddNumberToObject( root, "spo2", patientinfo->spo2 );
    cJSON_AddNumberToObject( root, "time", 0 );
        
    return root;
}


cJSON* updateRoot( cJSON* root, patientInfo_t* patientinfo )
{
    //cJSON_ReplaceItemInObject(root, "heap",
    //        cJSON_CreateNumber(system_get_free_heap_size()));
    cJSON_ReplaceItemInObject( root, "pulse", 
            cJSON_CreateNumber( patientinfo->pulse ) );
    cJSON_ReplaceItemInObject( root, "spo2", 
            cJSON_CreateNumber( patientinfo->spo2 ) );
            
    return root;
}