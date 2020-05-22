/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 DMA - CRC32 calculation using DMA
 *
 * Description: This code examples shows how to use the DMA module on MSP432 
 * to feed data into the CRC32 module for a CRC32 signature calculation. This
 * use case is particularly useful when the user wants to calculate the CRC32
 * signature of a large data array (such as a firmware image) but still wants
 * to maximize power consumption. After the DMA transfer is setup, a software
 * initiation occurs and the MSP432 device is put to sleep. Once the transfer
 * completes, the DMA interrupt occurs and the CRC32 result is placed into
 * a local variable for the user to examine.
 *
 *              MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* Statics */
static volatile uint32_t crcSignature;

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
uint8_t controlTable[1024];

uint8_t data_array[10240]; //Data array
int size_array[] = {512, 1024, 1030, 1824, 2048, 2049, 2303, 10240};  //Larger data sizes to transfer
int size;

#define CRC32_SEED              0xFFFFFFFF //CRC Seed

volatile int dma_done;  //Flag to signal when DMA is done

int main(void)
{
    uint32_t t0, t1, tHW, tDMA;
    float tHW_us, tDMA_us, speedup;

    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    /* Setting Control Indexes. In this case we will set the source of the
     * DMA transfer to our random data array and the destination to the
     * CRC32 data in register address*/
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
            UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);

    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_Interrupt_enableMaster();

    /* Setup a timer */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,
    TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    uint32_t clk_freq = MAP_CS_getMCLK();                       //Gets the clock frequency
    printf("\nClock frequency: %uHz\n\n", clk_freq);

    int jj;
    for (jj = 0; jj < 8; jj++)      //For loop to iterate over each data size
    {
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);  //t0

        /* Copy data to accelerator */
        /* Hardware method of computing checksum */
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        int ii;
        for (ii = 0; ii < size_array[jj]; ii++)
         MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);  //t1
        tHW = t0 - t1;

        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);  //t0
        /* Reinitialize result register in CRC32 accelerator */
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

        size = size_array[jj]; //Get size of data array

        do
        {
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                    UDMA_MODE_AUTO, data_array + (jj * 1024),
                    (void*) (&CRC32->DI32), (size >= 1024) ? 1024 : size); //Number of data items updated
            //Is size greater than or equal to 1024? If it is, transfer 1024. Otherwise, transfer rest of size
    
            /* Enabling DMA Channel 0 */
            MAP_DMA_enableChannel(0); //Channel disabled after DMA finished; Must be called

            dma_done = 0; //Reset DMA flag before transfer

            /* Forcing a software transfer on DMA Channel 0 */
            MAP_DMA_requestSoftwareTransfer(0);
            while(!dma_done)
            {

            }
            size -= 1024;
        }while(size>0);

        crcSignature = MAP_CRC32_getResult(CRC32_MODE);     //Get result

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);  //t1
        tDMA = t0 - t1;

        tHW_us = ((float)tHW/(float)clk_freq)*1000000;
        tDMA_us = ((float)tDMA/(float)clk_freq)*1000000;
        speedup = (float)tHW/(float)tDMA;

        printf("Block size: %d\n", size_array[jj]);     //Block size
        printf("HW CRC:   %08x\n", hwCRC);                //HW CRC
        printf("DMA CRC:  %08x\n", crcSignature);         //DMA CRC
        printf("HW Time:  %fus\n", tHW_us);             //HW Time
        printf("DMA Time: %fus\n", tDMA_us);            //DMA Time
        printf("Speedup:  %f\n\n", speedup);             //Speedup
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    dma_done = 1;
}
