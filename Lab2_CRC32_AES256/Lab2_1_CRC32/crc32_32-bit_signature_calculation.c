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
 * MSP432 CRC32 - CRC32 Calculation
 *
 * Description: In this example, the CRC32 module is used to calculate a CRC32
 * checksum in CRC32 mode. This value is compared versus a software calculated
 * checksum. The idea here is to have the user use the debugger to step through
 * the code and observe the CRC calculation in the debugger. Note that this
 * code example was made to use the standard CRC32 polynomial value of
 * 0xCBF43926.
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
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static uint8_t myData[10240];                   //Initialize size of myData to 10240
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, simpleSum;

//![Simple CRC32 Example] 
int main(void)
{
    uint32_t i;
        for (i = 0; i < 10240; i++)             //Iterates through each index (i) of myData
        {
            do
                {
                    myData[i] = rand();         //Sets myData[i] to a random value
                }while(myData[i]%2 != i%2);     //Checks if myData and index are odd or even
                                                //Repeats the loop if both are not odd or both are not even
        }

        for (i = 0; i < 25; i++)
        {
            printf("%u ", myData[i]);           //Prints the contents of myData[i]
        }
        printf("\n\n");

    uint32_t ii;

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    /* Initialize timing variables */
    uint32_t t0, t1, tHW, tSW, tSim;
    double tHW_us, tSW_us, tSim_us;

    /* Setup a timer */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,
    TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    uint32_t clk_freq = MAP_CS_getMCLK();                       //Gets the clock frequency
    printf("Clock frequency: %u\n\n", clk_freq);


    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < 10240; ii++)                      //ii updated to 10240
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    tHW = t0 - t1;

    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    /* Calculating the CRC32 checksum through software */
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 10240);     //length updated to 10240
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    tSW = t0 - t1;

    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    /* Calculating the checksum through addition */
    simpleSum = compute_simple_checksum((uint8_t*) myData, 10240);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    tSim = t0 - t1;

    /* Calculate time in microseconds for each checksum computation */
    tSim_us = ((double)tSim/(double)clk_freq)*1000000;
    tHW_us = ((double)tHW/(double)clk_freq)*1000000;
    tSW_us = ((double)tSW/(double)clk_freq)*1000000;

    /* Print Information */
    printf("Simple Sum: %u\n", simpleSum);
    printf("Cycles: %u\tTime: %fus\n\n", tSim, tSim_us);
    printf("HW CRC: %u\n", hwCalculatedCRC);
    printf("Cycles: %u\tTime: %fus\n\n", tHW, tHW_us);
    printf("SW CRC: %u\n", swCalculatedCRC);
    printf("Cycles: %u\tTime: %fus\n\n", tSW, tSW_us);
    printf("Time difference: %dus\nSpeedup: %f\n\n", ((int)tSW_us - (int)tHW_us), (tSW_us/tHW_us));

    /* Print values before LSB is changed */
    printf("myData[20]: %u  myData[21]: %u\n", myData[20], myData[21]);

    uint32_t hw20, sim20, hw21, sim21;

    /* Get HW and Simple checksum when myData[20] is modified */
    myData[20] = myData[20] ^ 1;

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting results */
    hw20 = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    sim20 = compute_simple_checksum((uint8_t*) myData, 10240);

    /* Get HW and Simple checksum when myData[21] is additionally modified */
    myData[21] = myData[21] ^ 1;

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting results */
    hw21 = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    sim21 = compute_simple_checksum((uint8_t*) myData, 10240);

    printf("LSB change  %u              %u\n\n", myData[20], myData[21]);
    printf("No change\nHW: %u\tSimple: %u\n\n", hwCalculatedCRC, simpleSum);
    printf("myData[20] LSB change\nHW: %u\tSimple: %u\n\n", hw20, sim20);
    printf("myData[21] LSB change\nHW: %u\tSimple: %u\n\n", hw21, sim21);


    /* Pause for the debugger */
    __no_operation();
}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}

/* Simple Checksum */
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length)
{
    uint32_t ii;
    uint32_t checksum = 0;                  //Initializes checksum to 0

    for(ii=0;ii<length;ii++)                //Increments through each element of the data array
    {
        checksum = checksum + data[ii];     //Adds each element of the data array
    }

    return ~checksum;                       //Returns bit-reversed checksum
}
