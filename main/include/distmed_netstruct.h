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


/*******************************************************************************
*   Definitions
*******************************************************************************/

#define WIFI_SSID       CONFIG_WIFI_SSID
#define WIFI_PASS       CONFIG_WIFI_PASS
#define SSID_SIZE       32
#define PASS_SIZE       64

#define MDNS_HOSTNAME   CONFIG_MDNS_HOSTNAME
#define MDNS_INSTANCE   CONFIG_MDNS_INSTANCE
#define PORT_NUMBER     CONFIG_PORT_NUMBER

#define AP_SSID         CONFIG_AP_SSID
#define AP_PASS         CONFIG_AP_PASS

/*******************************************************************************
*   Structures and Variables
*******************************************************************************/

typedef struct
{
    char ssid[SSID_SIZE];
    char password[PASS_SIZE];
    char deviceIdentifier[64];
    tcpip_adapter_ip_info_t addressinfo;

} connection_info_t;

typedef struct
{
    char* destip;
    int port;
    int socketConnection;
    struct sockaddr_in remoteServer;
    
} serverAddress_info_t;