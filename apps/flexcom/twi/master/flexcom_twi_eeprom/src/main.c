/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main_sama5d2.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/
// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>

#define LED_ON()                        LED_GREEN_Set()
#define LED_OFF()                       LED_GREEN_Clear()

#define APP_AT24CM_DEVICE_ADDR         (0x54)
#define APP_AT24CM_MEMORY_ADDR         (0x0000)
#define APP_AT24CM_HIGH_MEMORY_ADDR    ((APP_AT24CM_MEMORY_ADDR >> 8) & 0xffu)
#define APP_AT24CM_LOW_MEMORY_ADDR     (APP_AT24CM_MEMORY_ADDR & 0xffu)
#define APP_AT24CM_PAGE_SIZE           (22)
#define APP_NUM_ADDRESS_BYTES          (2)
#define APP_TRANSMIT_DATA_LENGTH       (APP_AT24CM_PAGE_SIZE + APP_NUM_ADDRESS_BYTES)
#define APP_RECEIVE_DATA_LENGTH        (APP_AT24CM_PAGE_SIZE)
#define APP_ACK_DATA_LENGTH            (1)

static uint8_t testTxData[APP_TRANSMIT_DATA_LENGTH] =
{
    APP_AT24CM_HIGH_MEMORY_ADDR, APP_AT24CM_LOW_MEMORY_ADDR,
    'A', 'T', 'S', 'A', 'M', ' ', 'F', 'L', 'E', 'X', 'C', 'O', 'M', ' ',
    'T', 'W', 'I', ' ', 'D', 'e', 'm', 'o'
};

static uint8_t  testRxData[APP_RECEIVE_DATA_LENGTH];

typedef enum
{
    APP_STATE_EEPROM_STATUS_VERIFY,
    APP_STATE_EEPROM_WRITE,
    APP_STATE_EEPROM_WAIT_WRITE_COMPLETE,
    APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS,
    APP_STATE_EEPROM_READ,
    APP_STATE_EEPROM_WAIT_READ_COMPLETE,
    APP_STATE_VERIFY,
    APP_STATE_IDLE,
    APP_STATE_XFER_SUCCESSFUL,
    APP_STATE_XFER_ERROR

} APP_STATES;

typedef enum
{
    APP_TRANSFER_STATUS_IN_PROGRESS,
    APP_TRANSFER_STATUS_SUCCESS,
    APP_TRANSFER_STATUS_ERROR,
    APP_TRANSFER_STATUS_IDLE,

} APP_TRANSFER_STATUS;

void APP_FLEXCOMTWICallback(uintptr_t context )
{
    APP_TRANSFER_STATUS* transferStatus = (APP_TRANSFER_STATUS*)context;

    if(FLEXCOM0_TWI_ErrorGet() == FLEXCOM_TWI_ERROR_NONE)
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_ERROR;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    APP_STATES state = APP_STATE_EEPROM_STATUS_VERIFY;
    volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;
    uint8_t ackData = 0;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        /* Check the application's current state. */
        switch (state)
        {
            case APP_STATE_EEPROM_STATUS_VERIFY:

                /* Register the FLEXCOM TWI Callback with transfer status as context */
                FLEXCOM0_TWI_CallbackRegister( APP_FLEXCOMTWICallback, (uintptr_t)&transferStatus );

                /* Verify if EEPROM is ready to accept new requests */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                FLEXCOM0_TWI_Write(APP_AT24CM_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);

                state = APP_STATE_EEPROM_WRITE;
                break;

            case APP_STATE_EEPROM_WRITE:
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write 1 page of data to EEPROM */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    FLEXCOM0_TWI_Write(APP_AT24CM_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    state = APP_STATE_EEPROM_WAIT_WRITE_COMPLETE;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM is not ready to accept new requests */
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_EEPROM_WAIT_WRITE_COMPLETE:
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Read the status of internal write cycle */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    FLEXCOM0_TWI_Write(APP_AT24CM_DEVICE_ADDR, &testTxData[0], 2);
                    state = APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS:
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_EEPROM_READ;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    FLEXCOM0_TWI_Write(APP_AT24CM_DEVICE_ADDR, &testTxData[0], 2);
                }
                break;

            case APP_STATE_EEPROM_READ:
                /* Read the data from the page written earlier */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                FLEXCOM0_TWI_WriteRead(APP_AT24CM_DEVICE_ADDR, &testTxData[0], APP_NUM_ADDRESS_BYTES,  &testRxData[0], APP_RECEIVE_DATA_LENGTH);
                state = APP_STATE_EEPROM_WAIT_READ_COMPLETE;
                break;

            case APP_STATE_EEPROM_WAIT_READ_COMPLETE:
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_VERIFY;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_VERIFY:
                /* Verify the read data */
                if (memcmp(&testTxData[APP_NUM_ADDRESS_BYTES], &testRxData[0], APP_RECEIVE_DATA_LENGTH) == 0)
                {
                    /* It means received data is same as transmitted data */
                    state = APP_STATE_XFER_SUCCESSFUL;
                }
                else
                {
                    /* It means received data is not same as transmitted data */
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_XFER_SUCCESSFUL:

                LED_ON();
                break;

            case APP_STATE_XFER_ERROR:

                LED_OFF();
                break;

            default:
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

