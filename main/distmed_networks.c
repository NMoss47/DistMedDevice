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

#include "include/distmed_networks.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

const static char tag[] = "Network";
static EventGroupHandle_t wifi_event_group; //<! Wifi event group handler
const int CONNECTED_BIT = BIT0;             //<! Wifi connected status bit
serverAddress_info_t* serverAddress;        //<! Structure containing addr info


// TODO
//  SPIFFS save/get
//  Variable pass throughs
//  
/*void initializeWifi( void )
{
   // Check for saved connections, if they exist try to use em
   // If they dont exist goto APMode(); If they do - StationMode();
   
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    
    int rc;
    connection_info_t* connectionInfo;
    if( (rc = getConnectionInfo( &connectionInfo )) == -2)
    {
        
        ESP_LOGI(tag, "Connection info not found; starting AP mode" );
        APMode( );
        httpServer( );
        ESP_LOGI(tag, "Server ended; Starting station mode");
        stationMode( connectionInfo );
    }
    else    // We can expand this section...
    {
        ESP_LOGI(tag, "Starting station mode");
        printf("Size: %i\n\nWTF\n\n", sizeof(connectionInfo));
        //printf( "CONNECTION INFO\n%s\t%s\t%s", connectionInfo->ssid, connectionInfo->password,
        //                                                connectionInfo->deviceIdentifier );
        //stationMode( &connectionInfo ); 
    }
   
}*/

void initializeWifi( void )
{
    tcpip_adapter_init( );
    wifi_event_group = xEventGroupCreate( );
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT( );
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "DistMed",
            .password = "DistMedTest",
            .bssid_set = false
        }
    };
    
    ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_STA, &sta_config ) );
    ESP_ERROR_CHECK( esp_wifi_start( ) );
    ESP_ERROR_CHECK( esp_wifi_connect( ) );
}

esp_err_t event_handler( void* ctz, system_event_t* event )
{
    switch( event->event_id )
    {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGD( tag, "Wireless AP started." );
            xTaskCreatePinnedToCore( ( &httpServer ), "httpServer",
                                        8000, NULL, 5, NULL, 0 );
            break;
        
        case SYSTEM_EVENT_STA_START:
            ESP_LOGD( tag, "Connecting.." );
            //esp_wifi_connect( );
            break;
            
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits( wifi_event_group, CONNECTED_BIT );
            break;
            
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGD( tag, "Disconnected" );
            xEventGroupClearBits( wifi_event_group, CONNECTED_BIT );
            esp_wifi_connect( );
            break;
        
        default:
            break;
    }
    
    return ESP_OK;
}

void APMode( void )
{
    ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_AP ) );
    wifi_config_t apConfig = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = 0,
            .password = AP_PASS,
            .channel = 0,
            .authmode = WIFI_AUTH_OPEN, // Change if we are actually using pass
            .ssid_hidden = 0,
            .max_connection = 2,
            .beacon_interval = 100
        }
    };
    
    ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_AP, &apConfig ) );
    ESP_ERROR_CHECK( esp_wifi_start( ) );
}

void stationMode( connection_info_t* connectionInfo )
{
    ESP_LOGI( tag, "StationMode" );
    assert( strlen( connectionInfo->ssid ) > 0 );
    ESP_LOGI( tag, "%s", connectionInfo->ssid );
    
    if( connectionInfo->addressinfo.ip.addr != 0 )
    {
        tcpip_adapter_dhcpc_stop( TCPIP_ADAPTER_IF_STA );
        tcpip_adapter_set_ip_info( TCPIP_ADAPTER_IF_STA,
                                    &connectionInfo->addressinfo );
    }
    else
    {
        tcpip_adapter_dhcpc_start( TCPIP_ADAPTER_IF_STA );
    }
    
    ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA) );
    wifi_config_t sta_config;
    sta_config.sta.bssid_set = 0;
    
    memcpy( sta_config.sta.ssid, connectionInfo->ssid, SSID_SIZE );
    memcpy( sta_config.sta.password, connectionInfo->password, PASS_SIZE );
    ESP_LOGI( tag, "TEST POINT CHECK" );
    ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_STA, &sta_config ) );
    ESP_ERROR_CHECK( esp_wifi_start( ) );
    ESP_ERROR_CHECK( esp_wifi_connect( ) );
}

void getNetworkTime( void )
{
    xEventGroupWaitBits( wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY );
    
    sntp_setoperatingmode( SNTP_OPMODE_POLL );
    sntp_setservername( 0, CONFIG_SNTP_SERVER );
    sntp_init( );
    
    time_t current;
    struct tm timeinfo;
    time( &current );
    localtime_r( &current, &timeinfo );
    
    while( timeinfo.tm_year > (2017-1990) )
    {
        vTaskDelay( 5000 / portTICK_PERIOD_MS );
        time( &current );
        localtime_r( &current, &timeinfo );
    }
    
    //--------------------------------------------------------------------------
    // Not sure if all this here is needed...
    char buffer[100];
    printf( "UTC time\n" );
    strftime( buffer, sizeof( buffer ), "%d/%m/%Y %H:%M:%S", &timeinfo );
    printf( "\n" );
    printf( buffer );
    // log/print the time
    // here we can change the timezone
    setenv( "TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1 );
    tzset( );
    
    printf( "Time in italy..:\n" );
    localtime_r( &current, &timeinfo );
    strftime( buffer, sizeof( buffer ), "%d/%m/%Y %H:%M:%S", &timeinfo );
    printf( "%s\n", buffer );
    
    while( 1 )
    {
        printf( "Just waiting now...\n" );
        time( &current );
        localtime_r( &current, &timeinfo );
        strftime( buffer, sizeof( buffer ), "%d/%m/%Y %H:%M:%S", &timeinfo );
        printf( buffer );
        printf( "\n" );
        vTaskDelay( 1000/ portTICK_RATE_MS );
    } // just keep pinging that thing x-seconds later
    //
    //--------------------------------------------------------------------------
}

void mdnsServer( void )
{
    mdns_server_t* mdns = NULL;
    
    while( 1 )
    {
        if( !mdns )
        {
            esp_err_t error = mdns_init( TCPIP_ADAPTER_IF_STA, &mdns );
            if( error )
            {
                ESP_LOGE( tag, "Failed to initialzie mDNS: %x", error );
            }
            
            ESP_ERROR_CHECK( mdns_set_hostname( mdns, MDNS_HOSTNAME) );
            ESP_ERROR_CHECK( mdns_set_instance( mdns, MDNS_INSTANCE) );
            ESP_ERROR_CHECK( mdns_service_add( mdns, "_distmed", "_tcp",
                                                PORT_NUMBER ) );
        }
        else
        {
            queryService( mdns, "_distmedserv", "_tcp" );
        }
        
        vTaskDelay( 10000 / portTICK_PERIOD_MS );
    }
}

void queryService( mdns_server_t* mdns, char* service, char* proto )
{
    if( !mdns )
    {
        ESP_LOGD( tag, "no mDNS information given" );
        return;
    }
    
    uint32_t result;
    result = mdns_query( mdns, service, proto, 1000 );
    
    if( result )
    {
        size_t i;
        
        for( i = 0; i < result; i++ )
        {
            const mdns_result_t *r = mdns_result_get( mdns, i );
            if( r )
            {
                // Memcpy since the information is a string
                memcpy(serverAddress->destip, r->host, strlen(r->host));
                serverAddress->port = r->port;

            }
        }
        
        mdns_result_free( mdns );
    }
    else
    {
        ESP_LOGD( tag, "No hosts found!" );
    }
}


int createSocket( serverAddress_info_t* serverAddress )
{
    // Set up the socket
    serverAddress->socketConnection = socket( 
                                        AF_INET, SOCK_STREAM, IPPROTO_TCP );

    serverAddress->remoteServer.sin_family = AF_INET;
    
    ESP_LOGI( tag, "destip: %s\tport: %d",
                serverAddress->destip, serverAddress->port );
    // Set up the desination information
    inet_pton( AF_INET, serverAddress->destip, 
                    &serverAddress->remoteServer.sin_addr.s_addr );
    serverAddress->remoteServer.sin_port = htons( serverAddress->port );
    // Start the connection
    int connection = connect( serverAddress->socketConnection,
                            ( struct sockaddr * )&serverAddress->remoteServer,
                            sizeof( struct sockaddr_in ) );
    
    ESP_LOGI( tag, "Socket created: %d", serverAddress->socketConnection );                            
    return connection;
}

int sendDataToSocket( serverAddress_info_t* serverAddress, char* data )
{
    
    int connection = send( serverAddress->socketConnection,
                            data, strlen( data ), 0 );
    ESP_LOGI( tag, "sendDataToSocket: socket return: %d", connection );
    ESP_LOGI( tag, "sendDataToSocket: data: %s", data );
    return connection;
}

int closeSocket( serverAddress_info_t* serverAddress )
{
    ESP_LOGI( tag, "socket: %d", serverAddress->socketConnection );
    int connection = close( serverAddress->socketConnection );
    ESP_LOGI( tag, "socketreturn: %d", connection );
    return connection;
}

