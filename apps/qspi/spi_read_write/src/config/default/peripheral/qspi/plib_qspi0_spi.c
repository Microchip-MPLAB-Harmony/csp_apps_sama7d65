/*******************************************************************************
  QSPI0 Peripheral Library Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_qspi0_spi.c

  Summary
    QSPI0 peripheral library interface when in SPI mode.

  Description

  Remarks:

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

#include "plib_qspi0_spi.h"
#include "string.h" // memmove
#include "interrupts.h"

volatile static qspi_spi_obj qspiObj;


void QSPI0_Initialize(void)
{
    // Reset and Disable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_SWRST_Msk | QSPI_CR_QSPIDIS_Msk;

    while((QSPI0_REGS->QSPI_SR & QSPI_SR_QSPIENS_Msk) != 0U)
    {
        /* Do Nothing */
    }

    // Pad Calibration Configuration
    QSPI0_REGS->QSPI_PCALCFG = (QSPI0_REGS->QSPI_PCALCFG & ~QSPI_PCALCFG_CLKDIV_Msk) |
                                                QSPI_PCALCFG_CLKDIV(7U);

    /* Start Pad Calibration */
    QSPI0_REGS->QSPI_CR = QSPI_CR_STPCAL_Msk;

    /* Wait for Pad Calibration complete */
    while((QSPI0_REGS->QSPI_SR & QSPI_SR_CALBSY_Msk) != 0U)
    {
        /* Do Nothing */
    }

    /* DLYCS  = 0x0 */
    /* DLYBCT = 0x0 */
    /* NBBITS = 0 */
    /* CSMODE = 0x0 */
    /* WDRBT  = 0 */
    /* SMM    = SPI */
    QSPI0_REGS->QSPI_MR = ( QSPI_MR_SMM_SPI | QSPI_MR_NBBITS(0U) );


    /* Wait for synchronization Busy */
    while((QSPI0_REGS->QSPI_SR & QSPI_SR_SYNCBSY_Msk) != 0U)
    {
        /* Do Nothing */
    }

    /* Update Configuration */
    QSPI0_REGS->QSPI_CR = QSPI_CR_UPDCFG_Msk;

    /* Wait for synchronization Busy */
    while((QSPI0_REGS->QSPI_SR & QSPI_SR_SYNCBSY_Msk) != 0U)
    {
        /* Do Nothing */
    }

    // Enable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_QSPIEN_Msk;

    while((QSPI0_REGS->QSPI_SR & QSPI_SR_QSPIENS_Msk) == 0U)
    {
        /* Do Nothing */
    }
}

bool QSPI0_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;

    /* Verify the request */
    if((qspiObj.transferIsBusy == false) && (((txSize > 0U) && (pTransmitData != NULL)) || ((rxSize > 0U) && (pReceiveData != NULL))))
    {
        isRequestAccepted = true;
        qspiObj.transferIsBusy = true;
        qspiObj.txBuffer = pTransmitData;
        qspiObj.rxBuffer = pReceiveData;
        qspiObj.rxCount = 0;
        qspiObj.txCount = 0;
        if(pTransmitData == NULL)
        {
            txSize = 0U;
        }
        if(pReceiveData == NULL)
        {
            rxSize = 0U;
        }
        if (rxSize > txSize)
        {
            qspiObj.dummySize = rxSize - txSize;
        }
        else
        {
            qspiObj.dummySize = 0U;
        }
        /* Flush out any unread data in SPI read buffer */
        (void)QSPI0_REGS->QSPI_RDR;

        /* Start the first write here itself, rest will happen in ISR context */
        if((QSPI0_REGS->QSPI_MR & QSPI_MR_NBBITS_Msk) == QSPI_MR_NBBITS_8_BIT)
        {
            if(qspiObj.txCount < txSize)
            {
                QSPI0_REGS->QSPI_TDR = *((uint8_t*)qspiObj.txBuffer);
                qspiObj.txCount++;
            }
            else if(qspiObj.dummySize > 0U)
            {
                QSPI0_REGS->QSPI_TDR = 0xFFU;
                qspiObj.dummySize--;
            }
            else
            {
                /* Do Nothing */
            }
        }
        else if((QSPI0_REGS->QSPI_MR & QSPI_MR_NBBITS_Msk) == QSPI_MR_NBBITS_16_BIT)
        {
            /* divide by 2 since dealing with 2-byte quantities here */
            txSize >>= 1;
            qspiObj.dummySize >>= 1;
            rxSize >>= 1;

            if (qspiObj.txCount < txSize)
            {
                QSPI0_REGS->QSPI_TDR = *((uint16_t*)qspiObj.txBuffer);
                qspiObj.txCount++;
            }
            else if (qspiObj.dummySize > 0U)
            {
                QSPI0_REGS->QSPI_TDR = 0xFFU;
                qspiObj.dummySize--;
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }

        qspiObj.rxSize = rxSize;
        qspiObj.txSize = txSize;

        if (rxSize > 0U)
        {
            /* Enable receive interrupt to complete the transfer in ISR context */
            QSPI0_REGS->QSPI_IER = QSPI_IER_RDRF_Msk;
        }
        else
        {
            /* Enable transmit interrupt to complete the transfer in ISR context */
            QSPI0_REGS->QSPI_IER = QSPI_IER_TDRE_Msk;
        }
    }

    return isRequestAccepted;
}

bool QSPI0_Write(void* pTransmitData, size_t txSize)
{
    return(QSPI0_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool QSPI0_Read(void* pReceiveData, size_t rxSize)
{
    return(QSPI0_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool QSPI0_TransferSetup (QSPI_TRANSFER_SETUP * setup )
{
    bool transfercheck = false;
    if (setup != NULL)
    {
        QSPI0_REGS->QSPI_SCR= (uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase;

        QSPI0_REGS->QSPI_MR = (QSPI0_REGS->QSPI_MR & ~QSPI_MR_NBBITS_Msk) | (uint32_t)setup->dataBits;

        transfercheck = true;
    }
    return transfercheck;
}

void QSPI0_CallbackRegister (QSPI_CALLBACK callback, uintptr_t context)
{
    qspiObj.callback = callback;
    qspiObj.context = context;
}

bool QSPI0_IsBusy(void)
{
    return ((qspiObj.transferIsBusy) || ((QSPI0_REGS->QSPI_ISR & QSPI_ISR_TXEMPTY_Msk ) == 0U));
}

bool QSPI0_IsTransmitterBusy(void)
{
    return ((QSPI0_REGS->QSPI_ISR & QSPI_ISR_TXEMPTY_Msk) == 0U)? true : false;
}

void __attribute__((used)) QSPI0_InterruptHandler(void)
{
    uint32_t dataBits ;
    uint32_t receivedData;
    static bool isLastByteTransferInProgress = false;

    /* Additional temporary variables used to prevent MISRA violations (Rule 13.x) */
    uintptr_t context = qspiObj.context;
    uint32_t rxSize = qspiObj.rxSize;
    uint32_t txSize = qspiObj.txSize;

    dataBits = QSPI0_REGS->QSPI_MR & QSPI_MR_NBBITS_Msk;

    /* See if there's received data (MOSI) to be processed */
    if ((QSPI0_REGS->QSPI_ISR & QSPI_ISR_RDRF_Msk ) == QSPI_ISR_RDRF_Msk)
    {
        receivedData = (QSPI0_REGS->QSPI_RDR & QSPI_RDR_RD_Msk) >> QSPI_RDR_RD_Pos;

        if (qspiObj.rxCount < rxSize)
        {
            if(dataBits == QSPI_MR_NBBITS_8_BIT)
            {
                uint8_t* rxBuffer = (uint8_t*)qspiObj.rxBuffer;
                rxBuffer[qspiObj.rxCount] = (uint8_t)receivedData;
            }
            else
            {
                uint16_t* rxBuffer = (uint16_t*)qspiObj.rxBuffer;
                rxBuffer[qspiObj.rxCount] = (uint16_t)receivedData;
            }
            qspiObj.rxCount++;
        }
    }

    /* If there are more words to be transmitted, then transmit them here and keep track of the count */
    if((QSPI0_REGS->QSPI_ISR & QSPI_ISR_TDRE_Msk) == QSPI_ISR_TDRE_Msk)
    {
        /* Disable the TDRE interrupt. This will be enabled back if more than
         * one byte is pending to be transmitted */
        QSPI0_REGS->QSPI_IDR = QSPI_IDR_TDRE_Msk;

        if(dataBits == QSPI_MR_NBBITS_8_BIT)
        {
            if (qspiObj.txCount < txSize)
            {
                uint8_t* txBuffer = (uint8_t*)qspiObj.txBuffer;
                QSPI0_REGS->QSPI_TDR = txBuffer[qspiObj.txCount];
                qspiObj.txCount++;
            }
            else if (qspiObj.dummySize > 0U)
            {
                QSPI0_REGS->QSPI_TDR = 0xFFU;
                qspiObj.dummySize--;
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            if (qspiObj.txCount < txSize)
            {
                uint16_t* txBuffer = (uint16_t*)qspiObj.txBuffer;
                QSPI0_REGS->QSPI_TDR = txBuffer[qspiObj.txCount];
                qspiObj.txCount++;
            }
            else if (qspiObj.dummySize > 0U)
            {
                QSPI0_REGS->QSPI_TDR = 0xFFU;
                qspiObj.dummySize--;
            }
            else
            {
                /* Do Nothing */
            }
        }
        /* Additional temporary variable used to prevent MISRA violations (Rule 13.x) */
        bool dummyTxComplete = (qspiObj.dummySize == 0U);
        if ((qspiObj.txCount == txSize) && dummyTxComplete)
        {
            /* At higher baud rates, the data in the shift register can be
             * shifted out and TXEMPTY flag can get set resulting in a
             * callback been given to the application with the SPI interrupt
             * pending with the application. This will then result in the
             * interrupt handler being called again with nothing to transmit.
             * To avoid this, a software flag is set, but
             * the TXEMPTY interrupt is not enabled until the very end.
             */

            isLastByteTransferInProgress = true;
            /* Set Last transfer to deassert NPCS after the last byte written in TDR has been transferred. */
            QSPI0_REGS->QSPI_CR = QSPI_CR_LASTXFER_Msk;
        }
        else if (qspiObj.rxCount == rxSize)
        {
            /* Enable TDRE interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            QSPI0_REGS->QSPI_IDR = QSPI_IDR_RDRF_Msk;
            QSPI0_REGS->QSPI_IER = QSPI_IER_TDRE_Msk;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /* See if Exchange is complete */
    /* Additional temporary variable used to prevent MISRA violations (Rule 13.x) */
    bool txEmptySet = ((QSPI0_REGS->QSPI_ISR & QSPI_ISR_TXEMPTY_Msk) == QSPI_ISR_TXEMPTY_Msk);
    if (isLastByteTransferInProgress && txEmptySet)
    {
        if (qspiObj.rxCount == rxSize)
        {
            qspiObj.transferIsBusy = false;

            /* Disable TDRE, RDRF and TXEMPTY interrupts */
            QSPI0_REGS->QSPI_IDR = QSPI_IDR_TDRE_Msk | QSPI_IDR_RDRF_Msk | QSPI_IDR_TXEMPTY_Msk;

            isLastByteTransferInProgress = false;

            if(qspiObj.callback != NULL)
            {
                qspiObj.callback(context);
            }
        }
    }
    if (isLastByteTransferInProgress)
    {
        /* For the last byte transfer, the TDRE interrupt is already disabled.
         * Enable TXEMPTY interrupt to ensure no data is present in the shift
         * register before application callback is called.
         */
        QSPI0_REGS->QSPI_IER = QSPI_IER_TXEMPTY_Msk;
    }

}
/*******************************************************************************
 End of File
*/