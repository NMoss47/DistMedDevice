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

#include "include/distmed_webserver.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

const static char tag[] = "Webserver";
int serverStopRequest = 0;  // REWORK

void webserverEventHandler( struct mg_connection *connection, int event,
                                    void* eventData )
{
    // Log the start of the handler
    
    switch( event )
    {
        case MG_EV_HTTP_REQUEST:
        {
            // Log the face that we got a request of some kind
        
            struct http_message *message = ( struct http_message * ) eventData;
            char* uri = mgStrToStr( message->uri );
            // Log the URI, please
            
            // If the user sets the information...
            if( strcmp ( uri, "/set" ) == 0 )
            {
                ESP_LOGI( tag, "Saving connection information." );
                
                connection_info_t connectionInfo;
                saveConnectionInfo( &connectionInfo );
                mg_send_head( connection, 200, 0, "Content-Type: text/plain" );
            }
            // Basic user request...
            else if( strcmp( uri, "/" ) == 0 )
            {
                ESP_LOGI( tag, "Sending HMTL page: bootpage.hmtl" );
                
                mg_send_head( connection, 200, sizeof( bootpage_html ),
                                "Content-Type: text/html" );
                mg_send( connection, bootpage_html, sizeof( bootpage_html ) );
            }
            // The post from the user...
            else if( strcmp( uri, "/apSelected" ) == 0 )
            {
                ESP_LOGI( tag, "Getting connection data!" );
                connection_info_t connectionInfo;
                
                mg_get_http_var( &message->body, "ssid", 
                                    connectionInfo.ssid, SSID_SIZE );
                mg_get_http_var( &message->body, "password",
                                    connectionInfo.password, PASS_SIZE );
                mg_get_http_var( &message->body, "device", 
                                    connectionInfo.deviceIdentifier, 64 );    // TODO - Hardcode this size limit in a defintion
                                    
                // For a specified IP/Mask/Gasteway field
                // What a mess! CLEAN THIS UP - THERE IS A BETTER WAY!!!
                //--------------------------------------------------------------
                char ipBuffer[20];  // A buffer for the ip information
                if( mg_get_http_var( &message->body, "ip", ipBuffer, sizeof( ipBuffer ) ) > 0 )
                {
                    //ESP_LOGI(tag, "%s", ipBuffer);
                    inet_pton(AF_INET, ipBuffer, &connectionInfo.addressinfo.ip);
                }
                else
                {
                    connectionInfo.addressinfo.ip.addr = 0;
                }
                
                if( mg_get_http_var( &message->body, "gw", ipBuffer, sizeof( ipBuffer ) ) > 0 )
                {
                    //ESP_LOGI(tag, "%s", ipBuffer);
                    inet_pton( AF_INET, ipBuffer, &connectionInfo.addressinfo.gw);
                }
                else
                {
                    connectionInfo.addressinfo.gw.addr = 0;
                }
                
                if( mg_get_http_var( &message->body, "netmask", ipBuffer, sizeof( ipBuffer ) ) > 0 )
                {
                    //ESP_LOGI(tag, "%s", ipBuffer);
                    inet_pton( AF_INET, ipBuffer, &connectionInfo.addressinfo.netmask);
                }
                else
                {
                    connectionInfo.addressinfo.netmask.addr = 0;
                }
                //--------------------------------------------------------------
                
                saveConnectionInfo( &connectionInfo );
                serverStopRequest = 1;
                ESP_LOGI( tag, "SSID: %s\tPassword: %s\tDevice: %s",
                            connectionInfo.ssid, connectionInfo.password,
                            connectionInfo.deviceIdentifier );
                            
                mg_send_head( connection, 200, sizeof( apSelected_html ),
                                "Content-Type: text/html" );
                mg_send( connection, apSelected_html, 
                            sizeof( apSelected_html ));
            }
            else
            {
                mg_send_head( connection, 404, 0, "Content-Type: text/plain" );
            }
        
            break;
        }
        default:
            break;
    }
}

void httpServer( void )
{
    struct mg_mgr manager;
    struct mg_connection* connection;
    
    ESP_LOGD( tag, "HTTP server starting..." );
    
    // serverStopRequest = 0;
    
    mg_mgr_init( &manager, NULL );
    connection = mg_bind( &manager, "80", webserverEventHandler );
    
    if( connection == NULL )
    {
        ESP_LOGD( tag, "No mongoose connection!" );
        mg_mgr_free( &manager );
        vTaskDelete( NULL );
        return;
    }
    
    mg_set_protocol_http_websocket( connection );
    
    while(!serverStopRequest)
    {
        mg_mgr_poll( &manager, 1000 );
    }
    ESP_LOGI(tag, "Webserver terminating..");
    mg_mgr_free( &manager );
    //serverStarted = 0;
    
    //vTaskDelete( NULL );
    return;
}

char* mgStrToStr( struct mg_str mongooseString )
{
    // Allocate memory to hold the string we are converting.
    char *returnString  = (char *) malloc(mongooseString.len +1);
    // Copy the original string
    memcpy(returnString, mongooseString.p, mongooseString.len);
    // Add a terminator to the string.
    returnString[mongooseString.len] = 0;
    
    return returnString;
}