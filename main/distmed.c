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

#include "include/distmed.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

char tag[] = "distmed";

// MAIN
int app_main( void )
{
    // Initialize non-volatile storage, and WIFI.
    nvs_flash_init( );
    //initializeSPIFFS( ); // TODO
    initializeWifi( );
    
    // Do I need this delay? initializeWifi() should block totally if not
    vTaskDelay( 1000 );
    //xTaskCreate( &mdnsServer, "mdnsServer", 2048, NULL, 5, NULL);
    xTaskCreate( ( &mainTask ), "mainTask", 4096, NULL, 5, NULL );
    
    // So long as the task doesn't stop we won't reach here.
    return 0;
}

// Main application task
void mainTask( void )
{
    // Hardcoded server address
    serverAddress_info_t serverAddress;
    serverAddress.destip = "192.168.1.136";
    serverAddress.port = 9999;
    
    // Hardcoded patient information
    patientInfo_t patientinfo; 
    patientinfo.pulse = 0;
    patientinfo.spo2 = 0;
    
    // Max301000 settings
    max30100_conf_t settings;
    settings.intrpts = 0x00;
    settings.intrptsEN = 0x00;
    settings.mode = MODE_SPO2_PULSE;
    settings.spo2 = SPO2_SAMP_100 | SPO2_LEDPW_1600_16;
    // Red LED is the MSBs
    // 0x88 going into the register (prev tests were at 0x66)
    settings.leds = ( (LED_AMP_CONTROL_27mA << 4) | LED_AMP_CONTROL_27mA );
    
    // Create the json data location and the json root structure.
    static char* jsondata;
    cJSON* root = createRoot( &patientinfo );
    
    // Run the mDNS task
    
    
    // Create the socket from returned information
    int connection = createSocket( &serverAddress );
    
    // Set up i2c driver
    i2cMasterInit();
    // Send setup commands to the MAX30100
    setMode( &settings );
    
    // -----
    // Task loop
    // Get/Process data -> Format as JSON -> Send through socket -> Repeat
    // -----
    while( 1 )
    {
        // Start detection subroutines
        getBodyData( &patientinfo );
        ESP_LOGI(tag, "patientInfo: %f / %f", patientinfo.pulse, patientinfo.spo2 );

        // Create the JSON structure
        root = updateRoot( root, &patientinfo);
        jsondata = cJSON_PrintUnformatted( root );
        
        // Send out the JSON structure
        connection = sendDataToSocket( &serverAddress, strcat(jsondata, "\n" ));
        
        // Free the resource
        free(jsondata);    

        // Delay so we don't spew packets everywhere.
        // We might not need this becuase of the internal timings of the state
        // machine in getBodyData()
        vTaskDelay(50);
    }
    
    // If we somehow break out of while (if this code gets revised) then we
    // should clean up. Destroy the socket and delete the task and its contents
    connection = closeSocket( &serverAddress );

    ESP_LOGI( tag, "EXITING MAIN TASK" );

    vTaskDelete( NULL );
}
