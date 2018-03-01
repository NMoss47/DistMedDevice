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

#include "include/distmed_storage.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/
const static char tag[] = "SPIFFS"; //"Storage";


void initializeSPIFFS( void )
{
    ESP_LOGI( tag, "Initialzing SPIFFS" );
    
    esp_vfs_spiffs_conf_t configuration = {
        .base_path = "/distmed",
        .partition_label = "DistMedPart",
        .max_files = 5, 
        .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register( &configuration );
    
    if( ret != ESP_OK )
    {
        switch( ret )
        {
            case ESP_FAIL:
                ESP_LOGE( tag, "Failed to mount or format the filesystem" );
                break;
            
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE( tag, "Failed to find the partition!" );
                break;
            
            default:
                ESP_LOGE( tag, "Failed to initialize spiffs!" );
                break;
        }

        return;
    }
}

void saveConnectionInfo( connection_info_t* connectionInfo )
{
    ESP_LOGI( tag, "Saving file!" );
    FILE* file = fopen( "/distmed/connection.txt", "w" );
    fclose( file );
    file = fopen( "/distmed/connection.txt", "w" );
    if(file == NULL)
    {
        ESP_LOGE( tag, "Failed to create file.." );
        return;
    }
    fprintf( file, connectionInfo->ssid );
    fprintf( file, connectionInfo->password );
    fprintf( file, connectionInfo->deviceIdentifier );
    fclose( file );
    
    dumpFileContents() ;
    
    ESP_LOGI( tag, "File written!" );
    
}

int getConnectionInfo( connection_info_t* connectionInfo )
{
    ESP_LOGI( tag, "Retriving information from file..." );
    FILE* file = fopen( "/distmed/connection.txt", "r" );
    if( file == NULL )
    {
        ESP_LOGE( tag, "Connection file not found!" );
        return -2;
    }
   
    char ssid[SSID_SIZE];
    char password[PASS_SIZE];
    char deviceIdentifier[64];
    
    fgets( ssid, sizeof( ssid ), file );
    fgets( password, sizeof( password ), file );
    fgets( deviceIdentifier, sizeof( deviceIdentifier ), file );
    
    fclose( file );
    
    // Strip newlines?
    char* pos = strchr( ssid, '\n' );
    if( pos )
        *pos = '\0';
    pos = strchr( password, '\n' );
    if( pos )
        *pos = '\0';
    pos = strchr( deviceIdentifier, '\n' );
    if( pos )
        *pos = '\0';
    
    memcpy( connectionInfo->ssid, ssid, sizeof( ssid ) );
    memcpy( connectionInfo->password, password, sizeof( password ) );
    memcpy( connectionInfo->deviceIdentifier, deviceIdentifier,
                            sizeof( deviceIdentifier ) );
    
    ESP_LOGI( tag, "Connection information retrieved!" );
    
    return 0;
}

void deleteConnectionFile( void )
{
    
}

void dumpFileContents( void )
{
    ESP_LOGI( tag, "Dumping contents" );
    FILE* file = fopen( "/distmed/connection.txt", "r" );
    if ( file == NULL )
    {
        ESP_LOGE( tag, "file not found" );
        return;
    }
    
    char buffer[1024];
    while( fgets( buffer, sizeof( buffer ), file ) != NULL )
    {
        printf( "%s\n", buffer );
    }
    
    fclose( file );
}