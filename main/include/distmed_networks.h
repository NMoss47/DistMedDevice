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

#ifndef distmed_networks
#define distmed_networks
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "apps/sntp/sntp.h"
#include "mdns.h"
#include "stdio.h"
#include "stdlib.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "string.h"
#include "distmed_netstruct.h"
#include "distmed_storage.h"
#include "distmed_webserver.h"


/*******************************************************************************
*   Definitions
*******************************************************************************/

// Definitions in distmed_netstruct.h

/*******************************************************************************
*   Structures and Variables
*******************************************************************************/

// Structures in distmed_netstruct.h

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This function is used to start the wifi mode for the device.
*/
void initializeWifi( void );

/**
* @brief This function is an event handler for the initializeWifi function. It
* can be used to switch between AP and STA modes to handle user input and
* connections.
*
* @param ctz Just a void.
* @param event A system event that the event handler, well, handles.
*/
esp_err_t event_handler(void* ctz, system_event_t* event );

/**
* @brief This function switches the device to become an access point and start
* a webserver.
*/
void APMode( void );

/**
* @brief This function switches the devices to become a wifi station. From there
* the device can start the rest of its subroutines.
*
* @param connection_info Connection_info_t structure used to initiate a
* connection.
*/
void stationMode( connection_info_t* connection_info );

/**
* @brief This function is used to configure the real time clock with the current
* network time once it has connected as a station. 
*/
void getNetworkTime( void );

/**
* @brief This function starts an mDNS server that can query for local services
* and broadcast its own services. Used to find a local DistMed Server.
*/
void mdnsServer( void );

/**
* @brief This function is used to locate some local services. 
* 
* @param mdnsserv The mDNS server structure.
* @param service The service being queried.
* @param proto The protocol of the service.
*/
void queryService( mdns_server_t* mdnsserv, char* service, char* proto );

/**
* @brief This function makes a network socket to communicate with a remote host.
*
* @param serverAddress A strcture of the remote server's IP and port.
* @return An int indicating the socket status.
*/
int createSocket( serverAddress_info_t* serverAddress );

/**
* @brief This function sends data to the specied socket.
*
* @param serverAddress A structure of the remote server's IP and port.
* @param data A pointer to the location of the data to be sent.
* @return An int indicating the socket status.
*/ 
int sendDataToSocket( serverAddress_info_t* serverAddress, char* data );

/**
* @brief This function closes a socket.
* 
* @param serverAddress The structure that contains the socket information.
* @return An int indicating the socket status.
*/
int closeSocket( serverAddress_info_t* serverAddress );

#endif