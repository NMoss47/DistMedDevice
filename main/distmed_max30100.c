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

#include "include/distmed_max30100.h"
#include "distmed_i2c.h"

/*******************************************************************************
*   Variable Declarations
*******************************************************************************/

static const char tag[] = "max30100";

// State machine implmentation to detect the pulse
static pulseStateMatrixRow_t pulseStateMatrix[] = {
    // Current state    // Event            // Next state
    // Start
    { PULSE_INIT,       EVENT_START,        PULSE_IDLE  },
    { PULSE_INIT,       EVENT_SIGNAL,       PULSE_IDLE  },
    { PULSE_IDLE,       EVENT_PULSE_UP,     PULSE_UP    },
    // Increasing
    { PULSE_UP,         EVENT_PULSE_DOWN,   PULSE_DOWN  },
    { PULSE_UP,         EVENT_OUT_BOUNDS,   PULSE_IDLE  },
    { PULSE_UP,         EVENT_PULSE_UP,     PULSE_UP    },
    // Decreasing
    { PULSE_DOWN,       EVENT_OUT_BOUNDS,   PULSE_IDLE  },
    { PULSE_DOWN,       EVENT_PULSE_UP,     PULSE_UP    },
    { PULSE_DOWN,       EVENT_PULSE_DOWN,   PULSE_DOWN  },
    // No input
    { PULSE_INIT,       EVENT_NO_FINGER,    PULSE_NOSIG },
    { PULSE_IDLE,       EVENT_NO_FINGER,    PULSE_NOSIG },
    { PULSE_UP,         EVENT_NO_FINGER,    PULSE_NOSIG },
    { PULSE_DOWN,       EVENT_NO_FINGER,    PULSE_NOSIG },
    // Recovery
    { PULSE_NOSIG,      EVENT_SIGNAL,       PULSE_IDLE  },
    // Exit
    { PULSE_IDLE,       EVENT_FINISHED,     EXIT_STATE  },
    { PULSE_UP,         EVENT_FINISHED,     EXIT_STATE  },
    { PULSE_DOWN,       EVENT_FINISHED,     EXIT_STATE  }
};

// Struct used to connect a state with its function
static pulseStateFunctionRow_t pulseStateFunctions[] = {
    // Name of the state    // Function
    { "PULSE_INIT",         &initPulseState         },
    { "PULSE_IDLE",         &pulseFunctionIdle      },
    { "PULSE_UP",           &pulseFunctionUp        },
    { "PULSE_DOWN",         &pulseFunctionDown      },
    { "PULSE_NOSIG",        &pulseFunctionNoSig     },
    { "EXIT_STATE",         &returnData             }
};

/*******************************************************************************
*   Function Definitions
*******************************************************************************/

// Root function
void getBodyData( patientInfo_t* patientinfo )
{
    ESP_LOGI( tag, "Getting body data..." );
    // Create the biological data
    pulseOximeter_t pulseox = update( );
    
    // Update our patientInfo_t struct and return to caller
    patientinfo->pulse = pulseox.bpm;
    patientinfo->spo2 = pulseox.sa02;
}

pulseOximeter_t update( )
{
    ESP_LOGI( tag, "Calling Update()!!" );

    // Setup our structures
    pulseStateMachine_t state;
    pulseStateEvent_t stateEvent;
    initPulseState( &state );

    pulseOximeter_t result = {
        0, 0, 0, 0, 0, 0, 0, 0
    };

    max30100_data_t maxdata;

    // These filters are static so they should still exist after update() exits
    static meanDiffFilter_t meanDiffIR = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Replace with loop
         0, 0, 0};
    static dcFilter_t dcFilterIR = { 0, 0 }; 
    static dcFilter_t dcFilterRed = { 0, 0 };
    static butterFilter_t lpbFilterIR = { {0, 0}, 0};

    // Initialize the structures
    initializeMaxData( &maxdata );

    rawFifoData_t fifoData;
    
    while( state.currentState != EXIT_STATE )
    {
        // Read the FIFO
        burstReadFIFO( I2C_NUM_1, &fifoData );

        // Signal processing
        // Remove the DC component from the signal
        dcFilterIR  = dcRemove( fifoData.irRaw,  dcFilterIR.w,  ALPHA );
        dcFilterRed = dcRemove( fifoData.redRaw, dcFilterRed.w, ALPHA );
        
        // Moving average filter for the IR signal
        float meanDiffResIR = meanDiff( dcFilterIR.result, &meanDiffIR );
        lpButterFilter( meanDiffResIR, &lpbFilterIR );

        // Set the filtered data to the max30100_data_t structure
        maxdata.previous = maxdata.current;
        maxdata.current = lpbFilterIR.result;

        // Add the squares of the filtered value to calculate RMS
        maxdata.irACValueSqSum  += dcFilterIR.result  * dcFilterIR.result;
        maxdata.redACValueSqSum += dcFilterRed.result * dcFilterRed.result;
        maxdata.samplesRecorded++;
        
        // ----- State switching ------
        // Create an event based off of the current and previous filtered values
        
        stateEvent = stateGetEvent( &maxdata, &fifoData );
        // This hacky statement ensures I can break PULSE_NOSIG state
        if( state.currentState == PULSE_NOSIG && fifoData.irRaw > 0x2FFF && fifoData.redRaw > 0x2FF) { stateEvent = EVENT_SIGNAL; }

        // Parse through the states and call their handling function
        for( int i = 0; i < 17; i++ )
        {
            if( pulseStateMatrix[i].currentState == state.currentState )
            {
                if( pulseStateMatrix[i].event == stateEvent )
                {
                    ESP_LOGI( tag, "STATE IS : %s",
                        pulseStateFunctions[state.currentState].name );

                    (pulseStateFunctions[state.currentState].func)( &maxdata );
                    
                    state.currentState = pulseStateMatrix[i].nextState;
                    break;
                }
            }
        }

        // Calculate the SPO2
        float ratioRMS = log( sqrt( maxdata.redACValueSqSum/maxdata.samplesRecorded ) ) / log( sqrt( maxdata.irACValueSqSum/maxdata.samplesRecorded ) );

        // Not perfect spo2 "adjustment"
        // No emperical data to calibrate the instrument so this linear
        // relationship will have to do for now
        maxdata.currentSaO2Value = 110.0 - 18.0 * ratioRMS;
        result.sa02 = maxdata.currentSaO2Value;

        // When we reach the intended sample size reset the values used
        // to calculate the spo2 information
        if( maxdata.pulsesDetected % PULSE_SAMPLE_SIZE )
        {
            maxdata.irACValueSqSum = 0;
            maxdata.redACValueSqSum = 0;
            maxdata.samplesRecorded = 0;
        }
        
        // This should delay for 10 milliseconds
        // Sampling frequency of 100Hz.
        vTaskDelay( 10 );
    }
    // ----- End the while( state.currentState != EXIT_STATE) -----

    // balance intensities
    //balanceLEDs( );

    // Dump the results into a structure and return them to the caller
    result.bpm = maxdata.currentBPM;
    result.ircardiogram = lpbFilterIR.result;
    result.irdcval = dcFilterIR.w;
    result.reddcval = dcFilterRed.w;
    result.lastbeatthreshold = maxdata.lastBeatThreshold;
    result.dcfilteredir = dcFilterIR.result;
    result.dcfilteredred = dcFilterRed.result;

    return result;
}


void initPulseState( pulseStateMachine_t* state )
{
    state->currentState = PULSE_IDLE;
}

void initializeMaxData( max30100_data_t* maxdata )
{
    ESP_LOGI( tag, "Initializing MaxData" );
    maxdata->current = 0;
    maxdata->previous = 0;
    maxdata->valuesDown = 0;
    maxdata->currentBeat = 0;
    maxdata->lastbeat = 0;

    maxdata->lastRedLEDCurrentCheck = 0;
    maxdata->currentBPM = 0;
    
    // Use a for loop to fill the BPM sample table with 0s
    for( int i = 0; i < PULSE_SAMPLE_SIZE; i++ )
    {
        maxdata->valuesBPM[i] = 0;
    }

    maxdata->valuesBPMSum = 0;
    maxdata->bpmIndex = 0;
    maxdata->lastBeatThreshold = 0;
    maxdata->irACValueSqSum = 0;
    maxdata->redACValueSqSum = 0;
    maxdata->samplesRecorded = 0;
    maxdata->pulsesDetected = 0;
    maxdata->currentSaO2Value = 0;
}

// TODO: change name
void returnData( max30100_data_t* maxdata )
{
    maxdata->pulsesDetected = 0;
}

void pulseFunctionIdle( max30100_data_t* maxdata )
{
    ESP_LOGI( tag, "pulseFunctionIdle" );
    if (maxdata->current >= PULSE_MIN_THRESHOLD )
        maxdata->valuesDown = 0;
    if( maxdata->current > PULSE_MAX_THRESHOLD )
    {
        maxdata->previous = 0;
        maxdata->lastbeat = 0;
        maxdata->currentBeat = 0;
        maxdata->valuesDown = 0;
        maxdata->lastBeatThreshold = 0;
    }
}

void pulseFunctionUp( max30100_data_t* maxdata )
{
    ESP_LOGI( tag, "pulseFunctionUp" );
    if( maxdata->current > maxdata->previous)
    {
        // xTaskGetTickCount() * portTICK_PERIOD_MS is the same as the arduino
        // function millis()
        maxdata->currentBeat = xTaskGetTickCount( ) * portTICK_PERIOD_MS; 
        maxdata->lastBeatThreshold = maxdata->current;
        ESP_LOGI( tag, "IF: setting tick value | %f / %f /%i",
            maxdata->current, maxdata->previous, maxdata->currentBeat );
    }
    // This section is not being used....WHY
    // If this else was to get used it would mean we have already shifted
    // into the PULSE_DOWN state, bypassing this thing entirely.
    else
    {
        uint32_t beatDuration = maxdata->currentBeat - maxdata->lastbeat;
        maxdata->lastbeat = maxdata->currentBeat;

        float rawbpm = 0;
        if( beatDuration > 0 )
            rawbpm = 60000.0 / (float)beatDuration;

        maxdata->valuesBPM[maxdata->bpmIndex] = rawbpm;
        maxdata->valuesBPMSum = 0;
        for( int i = 0; i < PULSE_SAMPLE_SIZE; i++ )
        {
            maxdata->valuesBPMSum += maxdata->valuesBPM[i];
        }

        maxdata->bpmIndex++;
        maxdata->bpmIndex = maxdata->bpmIndex % PULSE_SAMPLE_SIZE;

        if( maxdata->valuesBPMcount < PULSE_SAMPLE_SIZE )
            maxdata->valuesBPMcount++;

        maxdata->currentBPM = maxdata->valuesBPMSum / maxdata->valuesBPMcount;
        maxdata->pulsesDetected++;
        ESP_LOGI( tag, "ELSE: %d", maxdata->pulsesDetected );
    }
}

void pulseFunctionDown( max30100_data_t* maxdata )
{
    ESP_LOGI( tag, "pulseFunctionDown" );
    if( maxdata->current < maxdata->previous )
        maxdata->valuesDown++;
}

// I need to generate EVENT_SIGNAL to exit the NO FINGER state
pulseStateEvent_t stateGetEvent( max30100_data_t* maxdata, rawFifoData_t* fifo )
{
    ESP_LOGI( tag, "getStateEvent" );
    if( fifo->irRaw < 0x2FFF && fifo->redRaw < 0x2FFF )
    {
        ESP_LOGI( tag, "Event: No finger" );
        return EVENT_NO_FINGER;
    }
    if( maxdata->pulsesDetected >= PULSE_SAMPLE_SIZE )
    {
        ESP_LOGI( tag, "Event: EXIT" );
        return EXIT_STATE;
    }
    if( maxdata->current > PULSE_MAX_THRESHOLD )
    {
        ESP_LOGI( tag,"Event: OOB: GREATER %f / %f", maxdata->current, maxdata->previous );
        return EVENT_OUT_BOUNDS;
    }
    if( maxdata->current < PULSE_MIN_THRESHOLD )
    {
        ESP_LOGI( tag, "Event: OOB: LOWER: %f / %f", maxdata->current, maxdata->previous );
        return EVENT_OUT_BOUNDS;
    }
    if( maxdata->current > maxdata->previous && maxdata->current >= PULSE_MIN_THRESHOLD )
    {
        ESP_LOGI( tag, "Event: Pulse up: %f / %f", maxdata->current, maxdata->previous );
        return EVENT_PULSE_UP;
    }/*
    if( maxdata->current >= PULSE_MIN_THRESHOLD )
    {
        ESP_LOGI( tag, "Event: Pulse up: %f / %f",maxdata->current, maxdata->previous );
        return EVENT_PULSE_UP;
    }*/
    if( maxdata->current < maxdata->previous )
    {
        ESP_LOGI( tag, "Event: Pulse down: %f / %f", maxdata->current, maxdata->previous );
        return EVENT_PULSE_DOWN;
    }

    // We should never reach this point
    ESP_LOGI( tag, "Event: None" );
    return EVENT_NONE;
}

void pulseFunctionNoSig( max30100_data_t* maxdata )
{
    ESP_LOGI( tag, "No finger detected - Delaying (1) secs" );
    // Delay for some time and then retry
    int task1 = xTaskGetTickCount( );
    vTaskDelay( 100 );
    int task2 = xTaskGetTickCount( );
    maxdata->current = 0;
    maxdata->previous = 0;
    maxdata->currentBeat = 0;
    maxdata->lastbeat = 0;
    maxdata->pulsesDetected = 0;
    maxdata->samplesRecorded = 0;
    maxdata->currentSaO2Value = 0;
    ESP_LOGI(tag, "Tick counts for delay: %d", ( task2-task1 ) );

}

void balanceLEDs( float redLEDdc, float irLEDdc )
{

}

esp_err_t setLEDCurrents( uint8_t red, uint8_t ir )
{
    // Return the error code after sending LED config information
    return sendData( I2C_NUM_1 , W_ADDR,
                             LED_CONF_REG, ( ( red << 4 ) | ir ) );
}
