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
#include "distmed_netstruct.h"
#include "distmed_storage.h"
#include "apSelected_html.h"
#include "bootpage_html.h"
#include "mongoose.h"


/*******************************************************************************
*   Definitions
*******************************************************************************/


/*******************************************************************************
*   Structures and Variables
*******************************************************************************/



/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This function handles the events of the webserver. It handles basic 
* some basic http functionality.
* 
* @param connection The mongoose connection to handle.
* @param event The event to handle.
* @param eventData The data of the event.
*/
void webserverEventHandler(struct mg_connection* connection, int event,
                                    void* eventData);
                                    
/**
* @brief This is the webserver function of the project.
*/
void httpServer( void );

/**
* @brief This function is used to convert a mongoose string to a c-style string.
* 
* @param mongooseString The mongoose string to be converted.
*/
char *mgStrToStr( struct mg_str mongooseString );
