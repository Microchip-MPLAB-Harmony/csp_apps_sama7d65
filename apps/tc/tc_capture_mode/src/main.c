/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
//  This program demonstrates the capture feature of the Timer controller. It 
//  generates a PWM signal on one timer channel. The duty cycle of the signal 
//  is changed dynamically. Signal is then is provided as input to to another  
//  timer channel configured in capture mode and the measured duty cycle is 
//  output on the console. 
//  PWM signal is generated on Timer 1 Channel 0 (Pin PD30 accessible through
//  8th pin of connector J8). Signal is captured on Timer 0 Channel 2 (PIN PB22
//  accessible through 6th pin of connector J22).
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

/* Input waveform - compare value increment*/
#define CMP_VAL_INCREMENT (10000U)

/* Calculated duty cycle of the input waveform in %*/
float duty;
/* Calculated frequency of the input waveform in Hz*/
float frequency;

/* Interrupt counter */
volatile bool timer_expired = false;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/* This function is called after period expires */
void TC0_CH0_TimerInterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{
    timer_expired = true;
}

int main ( void )
{
    uint32_t period, on_time, off_time;
    uint32_t cmp_period, cmp_val=0; 
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    /* Register callback function for TC1 CH1 period interrupt, it is used for
       timing operations*/
    TC0_CH0_TimerCallbackRegister(TC0_CH0_TimerInterruptHandler, (uintptr_t)NULL);
    
    /* Start the timer channel 0*/
    TC0_CH0_TimerStart();

    cmp_period = TC0_CH1_ComparePeriodGet();
    TC1_CH0_CaptureStart();
    TC0_CH1_CompareStart();
    
    printf("\n\r---------------------------------------------------------");
    printf("\n\r                    TC Capture Demo                 ");
    printf("\n\r---------------------------------------------------------\n\r"); 
    
    printf("Connect TCx_CHx compare output to TCx_CHx capture input.\n\n\r");
    
    while ( true )
    {
        /* Change the duty cycle of the input waveform  */
        cmp_val += CMP_VAL_INCREMENT;
        if (cmp_val >= cmp_period)
        {
            cmp_val = CMP_VAL_INCREMENT;
        }
        TC0_CH1_CompareASet(cmp_val);
        
        /* Wait for compare event */
        while((TC1_CH0_CaptureStatusGet() & TC_CAPTURE_B_LOAD) != TC_CAPTURE_B_LOAD);
        
        /* Read Captured values */ 
        off_time = TC1_CH0_CaptureAGet();
        period = TC1_CH0_CaptureBGet();

        /* Compute Duty Cycle in percentage and Frequency in Hz */
        on_time = period - off_time;
        duty = ((on_time) * 100U) / period;
        frequency = (TC1_CH0_CaptureFrequencyGet() / period);
        
        /* Send the measured data to console for display  */
        printf("Frequency of waveform: %.2f Hz \t Duty Cycle of waveform: %.2f %%", frequency, duty);
        printf("\r\n"); 
                
        /* Wait for 1 second */ 
        while(!timer_expired);
        timer_expired = false;
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

