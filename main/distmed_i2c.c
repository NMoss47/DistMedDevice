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
#include "include/distmed_i2c.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "driver/i2c.h"

/*******************************************************************************
*   Function Definitions
*******************************************************************************/
const static char tag[] = "i2c";

void i2cMasterInit( void )
{
    // Create the 'port'
    int i2c_master_port = I2C_MASTER_NUM;
    
    // Create a configuration structure
    i2c_config_t configuration;
    
    // Set the values in the configuration structure
    configuration.mode              = I2C_MODE_MASTER;
    configuration.sda_io_num        = I2C_MASTER_SDA;
    configuration.sda_pullup_en     = GPIO_PULLUP_ENABLE;
    configuration.scl_io_num        = I2C_MASTER_SCL;
    configuration.scl_pullup_en     = GPIO_PULLUP_ENABLE;
    configuration.master.clk_speed  = I2C_MASTER_FREQ;
    
    // Load the configuration structure
    i2c_param_config( i2c_master_port, &configuration );
    
    // Install the i2c driver
    i2c_driver_install( i2c_master_port, configuration.mode,
                        RX_BUF_DISABLE, TX_BUF_DISABLE, 0 );
}


esp_err_t sendData( i2c_port_t i2c_num, uint8_t addr,
                            uint8_t reg, uint8_t data )
{
    
    i2c_cmd_handle_t handle = i2c_cmd_link_create( );
    i2c_master_start( handle );
    i2c_master_write_byte( handle, addr, ACK_CHECK_EN );
    i2c_master_write_byte( handle, reg,  ACK_CHECK_EN );
    i2c_master_write_byte( handle, data, ACK_CHECK_EN );
    i2c_master_stop( handle );
    

    int ret = i2c_master_cmd_begin( i2c_num, handle,
                                    ( 1000 / portTICK_RATE_MS ) );
    
    i2c_cmd_link_delete( handle );
    
    if( ret == ESP_FAIL )
    {
        //ESP_LOGE("SendData failed, error: %x", ret);
        return ret;
    }
    
    return ESP_OK; // or return ret?
}

esp_err_t setMode( max30100_conf_t* conf )
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create( );
    i2c_master_start( handle );
    
    // There is an internal pointer in the MAX30100 that auto increments the 
    // register I am accessing per command. Everytime write_byte is called
    // the pointer increments by one
    i2c_master_write_byte( handle, W_ADDR, ACK_CHECK_EN );
    i2c_master_write_byte( handle, 0x00, ACK_CHECK_EN ); // Point to first reg
    
    // Insert data in registers
    i2c_master_write_byte( handle, conf->intrpts,    ACK_CHECK_EN ); 
    i2c_master_write_byte( handle, conf->intrptsEN,  ACK_CHECK_EN );
    
    // FIFO registers
    i2c_master_write_byte( handle, 0x00, ACK_CHECK_EN );
    i2c_master_write_byte( handle, 0x00, ACK_CHECK_EN );
    i2c_master_write_byte( handle, 0x00, ACK_CHECK_EN );
    i2c_master_write_byte( handle, 0x00, ACK_CHECK_EN );
    
    // Configuration registers
    i2c_master_write_byte( handle, conf->mode,  ACK_CHECK_EN ); // mode
    i2c_master_write_byte( handle, conf->spo2,  ACK_CHECK_EN ); // spo2 config
    i2c_master_write_byte( handle, 0x00,        ACK_CHECK_EN ); // reserved
    i2c_master_write_byte( handle, conf->leds,  ACK_CHECK_EN ); // led config
    i2c_master_write_byte( handle, 0x00,        ACK_CHECK_EN ); // reserved
    

    i2c_master_stop( handle );
    int ret = i2c_master_cmd_begin( 
                    I2C_NUM_1, handle, ( 1000 / portTICK_RATE_MS ) );
    
    i2c_cmd_link_delete( handle );
    
    if( ret == ESP_FAIL )
    {
        //ESP_LOGE("SendData failed, error: %x", ret);
        return ret;
    }
    
    return ESP_OK; // or return ret?
}

// Deprecated
esp_err_t burstRead( i2c_port_t i2c_num, uint8_t addr, uint8_t reg, 
                            uint8_t* ir_out, uint8_t* ir_out2, 
                            uint8_t* r_out, uint8_t* r_out2 )
{

    i2c_cmd_handle_t handle = i2c_cmd_link_create( );
    
    i2c_master_start( handle );
    i2c_master_write_byte( handle, W_ADDR, ACK_CHECK_EN );
    i2c_master_write_byte( handle, reg, ACK_CHECK_EN );
    i2c_master_start( handle );
    
    i2c_master_write_byte( handle, R_ADDR , ACK_CHECK_EN );
    i2c_master_read_byte( handle, ir_out,  ACK_VAL );
    i2c_master_read_byte( handle, ir_out2, ACK_VAL );
    i2c_master_read_byte( handle, r_out,  ACK_VAL );
    i2c_master_read_byte( handle, r_out2, NACK_VAL );
    i2c_master_stop( handle );
    
    esp_err_t ret = i2c_master_cmd_begin( 
                        i2c_num, handle, 1000 / portTICK_RATE_MS );
    i2c_cmd_link_delete( handle );
    
    return ret;
}

// Rewrite this since the data sare just pointers
esp_err_t burstReadFIFO( i2c_port_t i2c_num , rawFifoData_t* fifodata )
{
    ESP_LOGI( tag, "burstReadFIFO()" );
    // Variables for the pointers returned during the commands
    uint8_t irmsb = 0;
    uint8_t irlsb = 0;
    uint8_t rmsb = 0;
    uint8_t rlsb = 0;
    
    // Create the handler
    i2c_cmd_handle_t handler = i2c_cmd_link_create( );
    
    // "Commands" to be issued.
    i2c_master_start( handler );
    i2c_master_write_byte( handler, W_ADDR, ACK_CHECK_EN );
    i2c_master_write_byte( handler, FIFO_DATA_REG , ACK_CHECK_EN );
    i2c_master_start( handler );
    i2c_master_write_byte( handler, R_ADDR, ACK_CHECK_EN );
    
    // Read the data returning from the MAX30100 
    i2c_master_read_byte( handler, &irmsb, ACK_VAL );
    i2c_master_read_byte( handler, &irlsb, ACK_VAL );
    i2c_master_read_byte( handler, &rmsb,  ACK_VAL );
    i2c_master_read_byte( handler, &rlsb, NACK_VAL );
    i2c_master_stop( handler );
    
    // Send queued commands/packets to i2c bus and return an error message
    esp_err_t errmsg = i2c_master_cmd_begin( 
                            i2c_num, handler, 1000 / portTICK_RATE_MS );
    
    // Delete the handler and free up some memory
    i2c_cmd_link_delete( handler );
    
    // The data retrieved are in 16bit units but the read function gets 8bits
    // Bitwise operations to convert the 8bit data chunks into a 16bit float
    fifodata->irRaw  = ( ( float ) ( (int)irmsb << 8 | (int)irlsb ) );
    fifodata->redRaw = ( ( float ) ( (int)rmsb << 8 | (int)rlsb ) );
    
    // Return the message
    return errmsg;
}
