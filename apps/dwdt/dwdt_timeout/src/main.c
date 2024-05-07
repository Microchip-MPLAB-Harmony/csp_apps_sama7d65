/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main_sam_a5d2.c

  Summary:
    This file contains a demonstration of the WDT periopheral for the SAM_A5D2.

  Description:
    A simple loop process blinks the LED for user feedback.  Within the loop
    the watch dog receives a 'pet' until the user presses the test push button. 
    The process will then starve the watch petting and the processor will reset.
    A spin lock creates a process starvation emulating a dead lock. 

    The PIT is used to provide a delay for the led blink and is not otherwise
    applicable to the demonstration

 *******************************************************************************/

/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                		// Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

volatile bool deadLockSwitchPressed = false;
volatile uint32_t tickleCounter = 0U;

void deadLockSet(PIO_PIN pin, uintptr_t context)
{
    (void)pin;
    (void)context;
    deadLockSwitchPressed = true;
}
void timerCallback(uintptr_t context)
{
    (void)context;
    tickleCounter++;
    if(!deadLockSwitchPressed)
    {
        LED_RED_Clear();
        LED_BLUE_Toggle();
    }
    else
    {
        LED_BLUE_Clear();
        LED_RED_Toggle();
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main( void )
{
    /* Initialize all modules */
    SYS_Initialize( NULL );
    PIO_PinInterruptCallbackRegister( USER_BUTTON_PIN, deadLockSet, 0U);
    PIO_PinInterruptEnable( USER_BUTTON_PIN );
    GENERIC_TIMER_CallbackRegister(timerCallback, 0U);

    printf( "\r\n-------------------------------------------------------------" );
    printf( "\r\n                          DWDT DEMO" );
    printf( "\r\n-------------------------------------------------------------" );
    printf( "\r\nFlashing LED indicates process is running\r\nPress SW2 " 
            "to simulate a deadlock\r\n");
    
    GENERIC_TIMER_Start();
    while( true )
    {
        if(!deadLockSwitchPressed)
        {
            if(tickleCounter > 2U)
            {
                DWDT_PS_Clear();
                tickleCounter = 0U;
            }
        }
        else
        {
            printf("\r\nProcess Starvation..............\r\nDevice Will Reset\r\n");
            while(true)
            {
                /* Spin forever */
            }
        }
    }

    /* Execution should not come here during normal operation */
    return( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/
