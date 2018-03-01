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

#ifndef distmed_max30100
#define distmed_max30100
#include "distmed_signalprocessing.h"
#include "esp_log.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"


/*******************************************************************************
*   Definitions
*******************************************************************************/

//----MAX30100------------------------------------------------------------------ 
//------------------------------------------------------------------------------

#define MA30100_ADDR            0x53    //<! 7-bit address of MAX30100
// The Write and Read "addresses" are just the main addresses with
// a bit shift. Ex: (0x53 << 0x01) for reading
#define R_ADDR                  0xAF    //<! MAX30100 addr with lsb read bit.
#define W_ADDR                  0xAE    //<! MAX30100 addr with lsb write bit.

// ----- Registers ------
// Status registers
#define INTERRUPT_REG           0x00    //<! Interrupt register
#define INTERRUPT_EN_REG        0x01    //<! Interrupt enable

// FIFO registers
#define FIFO_WRITE_REG          0x02    //<! FIFO write pointer
#define FIFO_OVERFLOW_REG       0x03    //<! FIFO overflow counter
#define FIFO_READ_REG           0x04    //<! FIFO read pointer
#define FIFO_DATA_REG           0x05    //<! FIFIO Data ( 8 bit samples )

// Configuration registers              //<! 
#define MODE_CONF_REG           0x06    //<! Mode configuration register
#define SPO2_CONF_REG           0x07    //<! SPO2 configuration register 
#define LED_CONF_REG            0x09    //<! LED configuration register

// Temperature registers                //<! T(meas) = T(int) + T(frac)
#define TEMP_INT_REG            0x16    //<! Integer temperature register
#define TEMP_FRAC_REG           0x17    //<! Fractional temperature register

//------------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Register values

#define MODE_PULSE              0x02    //<! Pulse mode
#define MODE_SPO2               0x03    //<! SPO2 mode
#define MODE_SPO2_PULSE         0x03    //<! SPO2 mode can do pulse as well

// -----------------------------------------------------------------------------
// SPO2_CONF_REG                [ X, HI_RES_EN, X, SPO2_SAMP, LEDPW ]
//                              [ (b7) (b6) (b5) (b4 b3 b2) (b1 b0) ] 

// These settings cannot be set randomly, refer to the MAX30100 datasheet.
// Check out pages 16, 19, and 20 of the ddatasheet.
// Sample rate control
#define SPO2_SAMP_50            0x40    //<! 50 samples per second
#define SPO2_SAMP_100           0x44    //<! 100 samples per second
#define SPO2_SAMP_167           0x48    //<! 167 samples per second
#define SPO2_SAMP_200           0x4C    //<! 200 samples per second
#define SPO2_SAMP_400           0x50    //<! 400 samples per second
#define SPO2_SAMP_600           0x54    //<! 600 samples per second
#define SPO2_SAMP_800           0x58    //<! 800 samples per second
#define SPO2_SAMP_1000          0x5C    //<! 1000 samples per second

// LED pulse width control
#define SPO2_LEDPW_200_13       0x00    //<! 200 us pulse width, 13 bit adc
#define SPO2_LEDPW_400_14       0x01    //<! 400 us pulse width, 14 bit adc
#define SPO2_LEDPW_800_15       0x02    //<! 800 us pulse width, 15 bit adc
#define SPO2_LEDPW_1600_16      0x03    //<! 1600 us pulse width, 16 bit adc

//------------------------------------------------------------------------------

// LED_CONF_REG                 [   RED_LED         IR_LED      ]
//                              [ (b7 b6 b5 b4) (b3, b2, b1 b0) ]

// These settings cannot be set randomly, refer to the MAX30100 datasheet.
// Check out pages 17 and 21 of the datasheet.
// LED current control
#define LED_AMP_CONTROL_0mA     0x00    //<! Set current to 0mA
#define LED_AMP_CONTROL_4mA     0x01    //<! Set current to 4mA
#define LED_AMP_CONTROL_7mA     0x02    //<! Set current to 7mA
#define LED_AMP_CONTROL_11mA    0x03    //<! Set current to 11mA
#define LED_AMP_CONTROL_14mA    0x04    //<! Set current to 14mA
#define LED_AMP_CONTROL_17mA    0x05    //<! Set current to 17mA
#define LED_AMP_CONTROL_20mA    0x06    //<! Set current to 20mA
#define LED_AMP_CONTROL_24mA    0x07    //<! Set current to 24mA
#define LED_AMP_CONTROL_27mA    0x08    //<! Set current to 27mA
#define LED_AMP_CONTROL_30mA    0x09    //<! Set current to 30mA
#define LED_AMP_CONTROL_33mA    0x0A    //<! Set current to 33mA
#define LED_AMP_CONTROL_37mA    0x0B    //<! Set current to 37mA
#define LED_AMP_CONTROL_40mA    0x0C    //<! Set current to 40mA
#define LED_AMP_CONTROL_43mA    0x0D    //<! Set current to 43mA
#define LED_AMP_CONTROL_46mA    0x0E    //<! Set current to 46mA
#define LED_AMP_CONTROL_50mA    0x0F    //<! Set current to 50mA

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Convient Settings
//#define SPO2_REG_DEFAULT        0x
//#define LED_AMP_DEFAULT         0x
// This function performs an OR operation to get the correct SPO2_CONF setting
// REDO THIS - MAYBE PUSH OUT AS ITS OWN FUNCTON
//#define SPO2_CONF( int, int )    int SPO2_CONF( int a, int b) { ( return ( a | b ); }

/*******************************************************************************
*   Structures
*******************************************************************************/

//-----Data-Structures----------------------------------------------------------

// Structure containing processed values of the biological measurements
typedef struct
{
    float bpm;              //<! Current BPM of the patient
    float ircardiogram;     //<! Result of post filtering IR value
    float irdcval;          //<! Result of DC filtered IR value
    float reddcval;         //<! Result of DC filtered Red value
    float sa02;             //<! Current SP02 of the patient
    int lastbeatthreshold;  //<! Last max value/beat of sensor
    float dcfilteredir;     //<! Result of DC filtering of IR signal
    float dcfilteredred;    //<! Result of DC filtering of Red signal
    
} pulseOximeter_t;

// This struct contains information to detect and calculate the heart beat
typedef struct
{   
    float current;                      //<! Current filtered signal value
    float previous;                     //<! Previous filtered signal value
    uint8_t valuesDown;                 //<! Count of number of times 
    
    uint32_t currentBeat;               //<! Time in ms of current detected max
    uint32_t lastbeat;                  //<! Time in ms of last beat max value

    float lastRedLEDCurrentCheck;       //<! Time since last LED calibration
    float currentBPM;                   //<! Current pulse of patient
    float valuesBPM[PULSE_SAMPLE_SIZE]; //<! Array of raw bpm values
    float valuesBPMSum;                 //<! Sum of all raw bpm values
    
    uint8_t valuesBPMcount;             //<! Count of values in valuesBPM
    uint8_t bpmIndex;                   //<! Index used to iterate valuesBPM

    // This was a uint32_t or something, not a float
    float lastBeatThreshold;            //<! Last "max" sensor value
    
    float irACValueSqSum;               //<! Used to calculate RMS
    float redACValueSqSum;              //<! Used to calculate RMS
    
    uint16_t samplesRecorded;           //<! Total amount of recorded samples
    uint16_t pulsesDetected;            //<! Total count of detected pulses
    float currentSaO2Value;             //<! Current SPO2 value

    // These are used for LED current control
    // NOT IMPLEMENTED
    //LEDCurrent irledcurrent;
    //uint8_t redLEDcurrent;

} max30100_data_t;

// Structure of the raw FIFO readings
typedef struct
{
    float irRaw;            //<! Raw red LED readings (uint16_t, originally)
    float redRaw;           //<! Raw IR LED readings (uint16_t, originally)
    
} rawFifoData_t;

// Structure containing filtered values for the red and IR LEDs
// NOT IMPLEMENTED
typedef struct
{
    float irval;            //<! Filtered values for the IR LED
    float rval;             //<! Filtered values for the red LED
    
} filteredValues_t;

// MAX30100 register configuration structure
typedef struct
{
    uint8_t intrpts;        //<! Data for MAX30100 interrupts register
    uint8_t intrptsEN;      //<! Data for MAX30100 interrupts enable register
    uint8_t mode;           //<! Data for MAX30100 operating mode
    uint8_t spo2;           //<! Data for MAX30100 spo2 configuration register
    uint8_t leds;           //<! Data for MAX30100 LED configuration register
    
} max30100_conf_t;

//-----End-Of-Data-Structures---------------------------------------------------

//-----State-Machine-Structures-------------------------------------------------

// Struct containing name of state and a function pointer
typedef struct
{
    const char* name;                           //<! Name of the state
    void (*func) ( max30100_data_t* maxdata );  //<! Pointer to state function

} pulseStateFunctionRow_t;

// The available states for pulse detection
typedef enum pulseState
{
    PULSE_INIT,         //<! First state of state machine
    PULSE_IDLE,         //<! Idle state, use if boundaries are exceeded
    PULSE_UP,           //<! State where pulse signals are increasing
    PULSE_DOWN,         //<! State where pulse signals are decrasing
    PULSE_NOSIG,        //<! State where there are no pulse signals
    EXIT_STATE          //<! State where processing is finished
    
} pulseState_t;

// Struct containing the current state
typedef struct
{
    pulseState_t currentState;  //<! The current state of the state machine

} pulseStateMachine_t;

// The events that allow us to transition between states
typedef enum
{
    EVENT_NONE,         //<! Just an empty event
    EVENT_START,        //<! Start event
    EVENT_PULSE_UP,     //<! Pulse signal value increasing
    EVENT_PULSE_DOWN,   //<! Pulse signal value decreasing
    EVENT_OUT_BOUNDS,   //<! Pulse signal outside of of max or min boundary
    EVENT_FINISHED,     //<! Pulses detected
    EVENT_FAIL,         //<! A failure event
    EVENT_NO_FINGER,    //<! No finger detected
    EVENT_SIGNAL        //<! Finger detected

} pulseStateEvent_t;

// A structure that maps a current state, an event, and the next state.
// Used in an array to do the state machine transitions.
typedef struct
{
    pulseState_t currentState;  //<! Current state
    pulseStateEvent_t event;    //<! Registered event
    pulseState_t nextState;     //<! Next state

} pulseStateMatrixRow_t;

//-----End-Of-State-Machine-Structures------------------------------------------

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/**
 * @brief This is the root function to get the body data. It serves to hold a
 * pointer to a struct that contains the information we want and calls functions
 * to generate that data.
 * @param patientinfo The struct pointer of the patient information
 */
void getBodyData( patientInfo_t* patientinfo);

pulseOximeter_t update( );


void balanceLEDs( float redLED, float irLED );

pulseStateEvent_t stateGetEvent( max30100_data_t* maxdata, rawFifoData_t* fifo );

void initPulseState( pulseStateMachine_t* state );

void returnData( max30100_data_t* maxdata );

void initializeMaxData( max30100_data_t* maxdata );

void pulseFunctionIdle( max30100_data_t* maxdata );

void pulseFunctionUp( max30100_data_t* maxdata );

void pulseFunctionDown( max30100_data_t* maxdata );

void pulseFunctionNoSig( max30100_data_t* maxdata );

#endif