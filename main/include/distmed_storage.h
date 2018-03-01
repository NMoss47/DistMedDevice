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
#ifndef distmed_storage
#define distmed_storage
#include "distmed_netstruct.h"
#include "nvs_flash.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"
#include "string.h"
#include "tcpip_adapter.h"
#include "esp_spiffs.h"
#include "sys/stat.h"
#include "esp_err.h"
#include "esp_vfs.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

#define SSID_SIZE 32
#define PASSWORD_SIZE 64
// NVS key names are limited to 15 characters, including null terminator

#define CONNECTION_NAMESPACE "connections"
#define CONNECTION_SSID "connSSID"
#define CONNECTION_INFO
#define CONNCOUNT "connCount"

/*******************************************************************************
*   Structures and Variables
*******************************************************************************/


/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
*
*
*/
void initializeSPIFFS( void );

/**
*
*
*/
void saveConnectionInfo( connection_info_t* connectionInfo );

/**
*
*
*/
int getConnectionInfo( connection_info_t* connectionInfo );

/**
*
*
*/
void deleteConnectionFile( void );

/**
*
*
*/
void dumpFileContents( void );


#endif