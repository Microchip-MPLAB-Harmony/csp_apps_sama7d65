/*******************************************************************************
  FLEXCOM6 USART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom6_usart.c

  Summary:
    FLEXCOM6 USART PLIB Implementation File

  Description
    This file defines the interface to the FLEXCOM6 USART
    peripheral library. This library provides access to and control of the
    associated peripheral instance.

  Remarks:
    None.
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#include "plib_flexcom6_usart.h"
#include "interrupts.h"

#define FLEXCOM_USART_RHR_8BIT_REG      (*(volatile uint8_t* const)((FLEXCOM6_BASE_ADDRESS + FLEX_US_RHR_REG_OFST)))
#define FLEXCOM_USART_RHR_9BIT_REG      (*(volatile uint16_t* const)((FLEXCOM6_BASE_ADDRESS + FLEX_US_RHR_REG_OFST)))

#define FLEXCOM_USART_THR_8BIT_REG      (*(volatile uint8_t* const)((FLEXCOM6_BASE_ADDRESS + FLEX_US_THR_REG_OFST)))
#define FLEXCOM_USART_THR_9BIT_REG      (*(volatile uint16_t* const)((FLEXCOM6_BASE_ADDRESS + FLEX_US_THR_REG_OFST)))

/* Disable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM6_USART_RX_INT_DISABLE()      FLEXCOM6_REGS->FLEX_US_IDR = (FLEX_US_IDR_RXRDY_Msk | FLEX_US_IDR_FRAME_Msk | FLEX_US_IDR_PARE_Msk | FLEX_US_IDR_OVRE_Msk)

/* Enable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM6_USART_RX_INT_ENABLE()       FLEXCOM6_REGS->FLEX_US_IER = (FLEX_US_IER_RXRDY_Msk | FLEX_US_IER_FRAME_Msk | FLEX_US_IER_PARE_Msk | FLEX_US_IER_OVRE_Msk)

#define FLEXCOM6_USART_TX_INT_DISABLE()      FLEXCOM6_REGS->FLEX_US_IDR = FLEX_US_IDR_TXRDY_Msk
#define FLEXCOM6_USART_TX_INT_ENABLE()       FLEXCOM6_REGS->FLEX_US_IER = FLEX_US_IER_TXRDY_Msk

#define FLEXCOM6_USART_READ_BUFFER_SIZE             20U
#define FLEXCOM6_USART_9BIT_READ_BUFFER_SIZE        (20U >> 1U)

#define FLEXCOM6_USART_WRITE_BUFFER_SIZE            128U
#define FLEXCOM6_USART_9BIT_WRITE_BUFFER_SIZE       (128U >> 1U)

volatile static uint8_t FLEXCOM6_USART_ReadBuffer[FLEXCOM6_USART_READ_BUFFER_SIZE];
volatile static uint8_t FLEXCOM6_USART_WriteBuffer[FLEXCOM6_USART_WRITE_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM6 USART Ring Buffer Implementation
// *****************************************************************************
// *****************************************************************************

volatile static FLEXCOM_USART_RING_BUFFER_OBJECT flexcom6UsartObj;

void FLEXCOM6_USART_Initialize( void )
{
    /* Set FLEXCOM USART operating mode */
    FLEXCOM6_REGS->FLEX_MR = FLEX_MR_OPMODE_USART;

    /* Reset FLEXCOM6 USART */
    FLEXCOM6_REGS->FLEX_US_CR = (FLEX_US_CR_RSTRX_Msk | FLEX_US_CR_RSTTX_Msk | FLEX_US_CR_RSTSTA_Msk);


    FLEXCOM6_REGS->FLEX_US_TTGR = 0;

    /* Enable FLEXCOM6 USART */
    FLEXCOM6_REGS->FLEX_US_CR = (FLEX_US_CR_TXEN_Msk | FLEX_US_CR_RXEN_Msk);

    /* Configure FLEXCOM6 USART mode */
    FLEXCOM6_REGS->FLEX_US_MR = ( FLEX_US_MR_USART_MODE_NORMAL | FLEX_US_MR_USCLKS_MCK | FLEX_US_MR_CHRL_8_BIT | FLEX_US_MR_PAR_NO | FLEX_US_MR_NBSTOP_1_BIT | (0UL << FLEX_US_MR_OVER_Pos));

    /* Configure FLEXCOM6 USART Baud Rate */
    FLEXCOM6_REGS->FLEX_US_BRGR = FLEX_US_BRGR_CD(54) | FLEX_US_BRGR_FP(2);

    flexcom6UsartObj.rdCallback = NULL;
    flexcom6UsartObj.rdInIndex = 0;
    flexcom6UsartObj.rdOutIndex = 0;
    flexcom6UsartObj.isRdNotificationEnabled = false;
    flexcom6UsartObj.isRdNotifyPersistently = false;
    flexcom6UsartObj.rdThreshold = 0;
    flexcom6UsartObj.rdBufferSize = FLEXCOM6_USART_READ_BUFFER_SIZE;
    flexcom6UsartObj.wrCallback = NULL;
    flexcom6UsartObj.wrInIndex = 0;
    flexcom6UsartObj.wrOutIndex = 0;
    flexcom6UsartObj.isWrNotificationEnabled = false;
    flexcom6UsartObj.isWrNotifyPersistently = false;
    flexcom6UsartObj.wrThreshold = 0;
    flexcom6UsartObj.wrBufferSize = FLEXCOM6_USART_WRITE_BUFFER_SIZE;
    flexcom6UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
    {
        flexcom6UsartObj.rdBufferSize = FLEXCOM6_USART_9BIT_READ_BUFFER_SIZE;
        flexcom6UsartObj.wrBufferSize = FLEXCOM6_USART_9BIT_WRITE_BUFFER_SIZE;
    }
    else
    {
        flexcom6UsartObj.rdBufferSize = FLEXCOM6_USART_READ_BUFFER_SIZE;
        flexcom6UsartObj.wrBufferSize = FLEXCOM6_USART_WRITE_BUFFER_SIZE;
    }

    FLEXCOM6_USART_RX_INT_ENABLE();
}

void static FLEXCOM6_USART_ErrorClear( void )
{
    /* Clear the error flags */
    FLEXCOM6_REGS->FLEX_US_CR = FLEX_US_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while((FLEXCOM6_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk) != 0U)
    {
        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            (void)(FLEXCOM_USART_RHR_9BIT_REG);
        }
        else
        {
            (void)(FLEXCOM_USART_RHR_8BIT_REG);
        }
    }
}

FLEXCOM_USART_ERROR FLEXCOM6_USART_ErrorGet( void )
{
    FLEXCOM_USART_ERROR errors = flexcom6UsartObj.errorStatus;

    flexcom6UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

static void FLEXCOM6_USART_BaudCalculate(uint32_t srcClkFreq, uint32_t reqBaud, uint8_t overSamp, uint32_t* cd, uint32_t* fp, uint32_t* baudError)
{
    uint32_t actualBaud = 0;

    *cd = srcClkFreq / (reqBaud * 8U * (2U - (uint32_t)overSamp));

    if (*cd > 0U)
    {
        *fp = ((srcClkFreq / (reqBaud * (2U - (uint32_t)overSamp))) - ((*cd) * 8U));
        actualBaud = (srcClkFreq / (((*cd) * 8U) + (*fp))) / (2U - overSamp);
        *baudError = ((100U * actualBaud)/reqBaud) - 100U;
    }
}

bool FLEXCOM6_USART_SerialSetup( FLEXCOM_USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    uint32_t cd0, fp0, cd1, fp1, baudError0, baudError1;
    bool status = false;

    cd0 = fp0 = cd1 = fp1 = baudError0 = baudError1 = 0U;

    if (setup != NULL)
    {
        baud = setup->baudRate;

        if(srcClkFreq == 0U)
        {
            srcClkFreq = FLEXCOM6_USART_FrequencyGet();
        }

        /* Calculate baud register values for 8x/16x oversampling values */

        FLEXCOM6_USART_BaudCalculate(srcClkFreq, baud, 0, &cd0, &fp0, &baudError0);
        FLEXCOM6_USART_BaudCalculate(srcClkFreq, baud, 1, &cd1, &fp1, &baudError1);

        if ( (!(cd0 > 0U && cd0 <= 65535U)) && (!(cd1 > 0U && cd1 <= 65535U)) )
        {
            /* Requested baud cannot be generated with current clock settings */
            return status;
        }

        if ( ((cd0 > 0U) && (cd0 <= 65535U)) && ((cd1 > 0U) && (cd1 <= 65535U)) )
        {
            /* Requested baud can be generated with both 8x and 16x oversampling. Select the one with less % error. */
            if (baudError1 < baudError0)
            {
                cd0 = cd1;
                fp0 = fp1;
                overSampVal = (1UL << FLEX_US_MR_OVER_Pos) & FLEX_US_MR_OVER_Msk;
            }
        }
        else
        {
            /* Requested baud can be generated with either with 8x oversampling or with 16x oversampling. Select valid one. */
            if ((cd1 > 0U) && (cd1 <= 65535U))
            {
                cd0 = cd1;
                fp0 = fp1;
                overSampVal = (1UL << FLEX_US_MR_OVER_Pos) & FLEX_US_MR_OVER_Msk;
            }
        }

        /* Configure FLEXCOM6 USART mode */
        usartMode = FLEXCOM6_REGS->FLEX_US_MR;
        usartMode &= ~(FLEX_US_MR_CHRL_Msk | FLEX_US_MR_MODE9_Msk | FLEX_US_MR_PAR_Msk | FLEX_US_MR_NBSTOP_Msk | FLEX_US_MR_OVER_Msk);
        FLEXCOM6_REGS->FLEX_US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | overSampVal);

        /* Configure FLEXCOM6 USART Baud Rate */
        FLEXCOM6_REGS->FLEX_US_BRGR = FLEX_US_BRGR_CD(cd0) | FLEX_US_BRGR_FP(fp0);

        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            flexcom6UsartObj.rdBufferSize = FLEXCOM6_USART_9BIT_READ_BUFFER_SIZE;
            flexcom6UsartObj.wrBufferSize = FLEXCOM6_USART_9BIT_WRITE_BUFFER_SIZE;
        }
        else
        {
            flexcom6UsartObj.rdBufferSize = FLEXCOM6_USART_READ_BUFFER_SIZE;
            flexcom6UsartObj.wrBufferSize = FLEXCOM6_USART_WRITE_BUFFER_SIZE;
        }

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool FLEXCOM6_USART_TxPullByte(void* pWrData)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = flexcom6UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom6UsartObj.wrInIndex;
    uint8_t* pWrByte = (uint8_t*)pWrData;
    if (wrOutIndex != wrInIndex)
    {
        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            uint32_t wrOutIdx = wrOutIndex << 1U;
            pWrByte[0] = FLEXCOM6_USART_WriteBuffer[wrOutIdx];
            pWrByte[1] = FLEXCOM6_USART_WriteBuffer[wrOutIdx + 1U];
        }
        else
        {
            *pWrByte = FLEXCOM6_USART_WriteBuffer[wrOutIndex];
        }
        wrOutIndex++;

        if (wrOutIndex >= flexcom6UsartObj.wrBufferSize)
        {
            wrOutIndex = 0U;
        }

        flexcom6UsartObj.wrOutIndex = wrOutIndex;
        isSuccess = true;
    }
    return isSuccess;
}

static inline bool FLEXCOM6_USART_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    uint32_t wrOutIndex;
    uint32_t wrInIndex;
    bool isSuccess = false;

    /* Take a snapshot of indices to avoid creation of critical section */
    wrOutIndex = flexcom6UsartObj.wrOutIndex;
    wrInIndex = flexcom6UsartObj.wrInIndex;

    tempInIndex = wrInIndex + 1U;
    if (tempInIndex >= flexcom6UsartObj.wrBufferSize)
    {
        tempInIndex = 0U;
    }
    if (tempInIndex != wrOutIndex)
    {
        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            uint32_t wrInIdx = wrInIndex << 1U;
            FLEXCOM6_USART_WriteBuffer[wrInIdx] = (uint8_t)wrByte;
            FLEXCOM6_USART_WriteBuffer[wrInIdx + 1U] = (uint8_t)(wrByte >> 8U);
        }
        else
        {
            FLEXCOM6_USART_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        flexcom6UsartObj.wrInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }
    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM6_USART_SendWriteNotification(void)
{
    uint32_t nFreeWrBufferCount;

    if (flexcom6UsartObj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = FLEXCOM6_USART_WriteFreeBufferCountGet();

        if(flexcom6UsartObj.wrCallback != NULL)
        {
            uintptr_t wrContext = flexcom6UsartObj.wrContext;

            if (flexcom6UsartObj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= flexcom6UsartObj.wrThreshold)
                {
                    flexcom6UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == flexcom6UsartObj.wrThreshold)
                {
                    flexcom6UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, wrContext);
                }
            }
        }
    }
}

static size_t FLEXCOM6_USART_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid creation of critical section */

    uint32_t wrOutIndex = flexcom6UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom6UsartObj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex -  wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (flexcom6UsartObj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t FLEXCOM6_USART_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = FLEXCOM6_USART_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t FLEXCOM6_USART_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    while (nBytesWritten < size)
    {
        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            uint16_t halfWordData = (uint16_t)(pWrBuffer[(2U * nBytesWritten) + 1U]);
            halfWordData <<= 8U;
            halfWordData |= (uint16_t)pWrBuffer[2U * nBytesWritten];
            if (FLEXCOM6_USART_TxPushByte(halfWordData) == true)
            {
                nBytesWritten++;
            }
            else
            {
                /* Queue is full, exit the loop */
                break;
            }
        }
        else
        {
            if (FLEXCOM6_USART_TxPushByte(pWrBuffer[nBytesWritten]) == true)
            {
                nBytesWritten++;
            }
            else
            {
                /* Queue is full, exit the loop */
                break;
            }
        }
    }

    /* Check if any data is pending for transmission */
    if (FLEXCOM6_USART_WritePendingBytesGet() > 0U)
    {
        /* Enable TX interrupt as data is pending for transmission */
        FLEXCOM6_USART_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t FLEXCOM6_USART_WriteFreeBufferCountGet(void)
{
    return (flexcom6UsartObj.wrBufferSize - 1U) - FLEXCOM6_USART_WriteCountGet();
}

size_t FLEXCOM6_USART_WriteBufferSizeGet(void)
{
    return (flexcom6UsartObj.wrBufferSize - 1U);
}

bool FLEXCOM6_USART_TransmitComplete(void)
{
    bool status = false;

    if ((FLEXCOM6_REGS->FLEX_US_CSR & FLEX_US_CSR_TXEMPTY_Msk) != 0U)
    {
        status = true;
    }

    return status;
}

bool FLEXCOM6_USART_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom6UsartObj.isWrNotificationEnabled;

    flexcom6UsartObj.isWrNotificationEnabled = isEnabled;

    flexcom6UsartObj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM6_USART_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        flexcom6UsartObj.wrThreshold = nBytesThreshold;
    }
}

void FLEXCOM6_USART_WriteCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom6UsartObj.wrCallback = callback;

    flexcom6UsartObj.wrContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool FLEXCOM6_USART_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = flexcom6UsartObj.rdInIndex + 1U;

    if (tempInIndex >= flexcom6UsartObj.rdBufferSize)
    {
        tempInIndex = 0U;
    }

    if (tempInIndex == flexcom6UsartObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(flexcom6UsartObj.rdCallback != NULL)
        {
            uintptr_t rdContext = flexcom6UsartObj.rdContext;

            flexcom6UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_BUFFER_FULL, rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = flexcom6UsartObj.rdInIndex + 1U;

            if (tempInIndex >= flexcom6UsartObj.rdBufferSize)
            {
                tempInIndex = 0U;
            }
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != flexcom6UsartObj.rdOutIndex)
    {
        uint32_t rdInIdx;

        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            rdInIdx = flexcom6UsartObj.rdInIndex << 1U;
            FLEXCOM6_USART_ReadBuffer[rdInIdx] = (uint8_t)rdByte;
            FLEXCOM6_USART_ReadBuffer[rdInIdx + 1U] = (uint8_t)(rdByte >> 8U);
        }
        else
        {
            rdInIdx = flexcom6UsartObj.rdInIndex;
            FLEXCOM6_USART_ReadBuffer[rdInIdx] = (uint8_t)rdByte;
        }

        flexcom6UsartObj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM6_USART_SendReadNotification(void)
{
    uint32_t nUnreadBytesAvailable;

    if (flexcom6UsartObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = FLEXCOM6_USART_ReadCountGet();

        if(flexcom6UsartObj.rdCallback != NULL)
        {
            uintptr_t rdContext = flexcom6UsartObj.rdContext;

            if (flexcom6UsartObj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= flexcom6UsartObj.rdThreshold)
                {
                    flexcom6UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == flexcom6UsartObj.rdThreshold)
                {
                    flexcom6UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, rdContext);
                }
            }
        }
    }
}

size_t FLEXCOM6_USART_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = 0;
    uint32_t rdInIndex = 0;


    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom6UsartObj.rdOutIndex;
    rdInIndex = flexcom6UsartObj.rdInIndex;

    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
            {
                uint32_t rdOutIdx = rdOutIndex << 1U;
                uint32_t nBytesReadIdx = nBytesRead << 1U;
                pRdBuffer[nBytesReadIdx] = FLEXCOM6_USART_ReadBuffer[rdOutIdx];
                pRdBuffer[nBytesReadIdx + 1U] = FLEXCOM6_USART_ReadBuffer[rdOutIdx + 1U];
                nBytesRead++;
                rdOutIndex++;
            }
            else
            {
                pRdBuffer[nBytesRead++] = FLEXCOM6_USART_ReadBuffer[rdOutIndex];
                rdOutIndex++;
            }

            if (rdOutIndex >= flexcom6UsartObj.rdBufferSize)
            {
                rdOutIndex = 0U;
            }
        }
        else
        {
            /* No more data available in the RX buffer */
            break;
        }
    }

    flexcom6UsartObj.rdOutIndex = rdOutIndex;

    return nBytesRead;
}

size_t FLEXCOM6_USART_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdOutIndex;
    uint32_t rdInIndex;


    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom6UsartObj.rdOutIndex;
    rdInIndex = flexcom6UsartObj.rdInIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (flexcom6UsartObj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t FLEXCOM6_USART_ReadFreeBufferCountGet(void)
{
    return (flexcom6UsartObj.rdBufferSize - 1U) - FLEXCOM6_USART_ReadCountGet();
}

size_t FLEXCOM6_USART_ReadBufferSizeGet(void)
{
    return (flexcom6UsartObj.rdBufferSize - 1U);
}


bool FLEXCOM6_USART_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom6UsartObj.isRdNotificationEnabled;

    flexcom6UsartObj.isRdNotificationEnabled = isEnabled;

    flexcom6UsartObj.isRdNotifyPersistently = isPersistent;


    return previousStatus;
}

void FLEXCOM6_USART_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        flexcom6UsartObj.rdThreshold = nBytesThreshold;

    }
}

void FLEXCOM6_USART_ReadCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom6UsartObj.rdCallback = callback;

    flexcom6UsartObj.rdContext = context;
}

void static __attribute__((used)) FLEXCOM6_USART_ISR_RX_Handler( void )
{
    uint16_t rdData = 0;

    /* Keep reading until there is a character availabe in the RX FIFO */
    while(FLEX_US_CSR_RXRDY_Msk == (FLEXCOM6_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk))
    {
        if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
        {
            rdData = FLEXCOM_USART_RHR_9BIT_REG & (uint16_t)FLEX_US_RHR_RXCHR_Msk;
        }
        else
        {
            rdData = (uint8_t)FLEXCOM_USART_RHR_8BIT_REG;
        }

        if (FLEXCOM6_USART_RxPushByte( rdData ) == true)
        {
            FLEXCOM6_USART_SendReadNotification();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }


}

void static __attribute__((used)) FLEXCOM6_USART_ISR_TX_Handler( void )
{
    uint16_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while(FLEX_US_CSR_TXRDY_Msk == (FLEXCOM6_REGS->FLEX_US_CSR & FLEX_US_CSR_TXRDY_Msk))
    {
        if (FLEXCOM6_USART_TxPullByte(&wrByte) == true)
        {
            if ((FLEXCOM6_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk) != 0U)
            {
                FLEXCOM_USART_THR_9BIT_REG = wrByte & (uint16_t)FLEX_US_THR_TXCHR_Msk;
            }
            else
            {
                FLEXCOM_USART_THR_8BIT_REG = (uint8_t)wrByte;
            }

            /* Send notification */
            FLEXCOM6_USART_SendWriteNotification();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty/fifo Threshold interrupt. */
            FLEXCOM6_USART_TX_INT_DISABLE();
            break;
        }
    }

}

void __attribute__((used)) FLEXCOM6_InterruptHandler( void )
{
    /* Channel status */
    uint32_t channelStatus = FLEXCOM6_REGS->FLEX_US_CSR;

    /* Error status */
    uint32_t errorStatus = (channelStatus & (FLEX_US_CSR_OVRE_Msk | FLEX_US_CSR_FRAME_Msk | FLEX_US_CSR_PARE_Msk));


    if(errorStatus != 0U)
    {
        /* Save the error so that it can be reported when application calls the FLEXCOM6_USART_ErrorGet() API */
        flexcom6UsartObj.errorStatus = (FLEXCOM_USART_ERROR)errorStatus;

        /* Clear the error flags and flush out the error bytes */
        FLEXCOM6_USART_ErrorClear();

        /* USART errors are normally associated with the receiver, hence calling receiver context */
        if( flexcom6UsartObj.rdCallback != NULL )
        {
            uintptr_t rdContext = flexcom6UsartObj.rdContext;

            flexcom6UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_ERROR, rdContext);
        }
    }

    /* Receiver status. RX interrupt is never disabled. */
    if ((channelStatus & FLEX_US_CSR_RXRDY_Msk) != 0U)
    {
        FLEXCOM6_USART_ISR_RX_Handler();
    }

    /* Transmitter status */
    if( ((channelStatus & FLEX_US_CSR_TXRDY_Msk) != 0U) && ((FLEXCOM6_REGS->FLEX_US_IMR & FLEX_US_IMR_TXRDY_Msk) != 0U) )
    {
        FLEXCOM6_USART_ISR_TX_Handler();
    }
}
