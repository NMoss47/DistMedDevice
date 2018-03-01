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
#include "distmed_max30100.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "driver/i2c.h"

/*******************************************************************************
*   Definitions
*******************************************************************************/

#define I2C_MASTER_SCL      GPIO_NUM_26      //!< GPIO pin for I2C Master SCL
#define I2C_MASTER_SDA      GPIO_NUM_25      //!< GPIO pin for I2C Master SDA
#define I2C_MASTER_NUM      I2C_NUM_1        //!< I2C port number of master
#define TX_BUF_DISABLE      0                //!< I2C master needs no TX buffer
#define RX_BUF_DISABLE      0                //!< I2C master needs no RX buffer
#define I2C_MASTER_FREQ     100000           //!< Frequency in Hertz

#define WRITE_BIT           I2C_MASTER_WRITE //!< I2C master write 
#define READ_BIT            I2C_MASTER_READ  //!< I2C master read 
#define ACK_CHECK_EN        0x1              //!< I2C master will check for ACK
#define ACK_CHECK_DIS       0x0              //!< I2C master will not check ACK
#define ACK_VAL             0x0              //!< Acknowledgement value  
#define NACK_VAL            0x1              //!< Non-acknowledgement value


/*******************************************************************************
*   Structures
*******************************************************************************/


/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
* @brief This function is used to initialzie the ESP32 as an I2C Master.
* It uses the defintions at the beginning of the header file.
*/
void i2cMasterInit( void );

/**
* @brief This function is used to send a commend through the I2C interface.
*
* @param i2c_num The I2C port to be used.
* @param addr The address of the module.
* @param reg The register of the module.
* @param data The data to be sent.
* @return Returns an esp_err_t struct to indicate if there was an error during
* the sending process.
*/
esp_err_t sendData( i2c_port_t i2c_num, uint8_t addr,
                              uint8_t reg, uint8_t data );

/**
* @brief This function is used to initialize the mode of the MAX30100.
* It should be redone.
*
* @param conf configuration structure for register settings. 
* @return Returns an esp_err_t struct to indicate if there was an error during
* the mode switching process.
*/
esp_err_t setMode( max30100_conf_t* conf );

/**
* @attention DEPRECATED
* @brief This function is used to burst read the FIFO registers of the MAX30100.
* 
* @param i2c_num The desired I2C port number.
* @param addr The address of the periperal.
* @param reg The desired register from which to read.
* @param ir_out An external buffer for the MSB of the 16-bit IR led value.
* @param ir_out2 An external buffer for the LSB of the 16-bit IR led value.
* @param r_out An external buffer for the MSB of the 16-bit Red led value.
* @param r_out2 An external buffer for the LSB of the 16-bit Red led value.
* @return Returns an esp_err_t struct to indicate if there was an error during
* the reading process. 
*/
esp_err_t burstRead( i2c_port_t i2c_num, uint8_t addr, uint8_t reg,
                            uint8_t* ir_out, uint8_t* ir_out2,
                            uint8_t* r_out, uint8_t* r_out2 );

/**
* @brief This function is used to burst read the FIFO registers of the MAX30100.
* 
* @param i2c_num The desired I2C port number.
* @param fifodata A struct that will hold the results fo the retrieved data.
* @return Returns an esp_err_t struct to indicate if there was an error during
* the reading process.
*/
esp_err_t burstReadFIFO( i2c_port_t i2c_num, rawFifoData_t* fifodata );
