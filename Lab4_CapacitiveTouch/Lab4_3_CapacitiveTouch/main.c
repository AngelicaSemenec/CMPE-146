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
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void delay_ms(uint32_t count);
uint32_t t0, t1, cycles;
uint16_t timer_A_cycles;

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Set up Timer32 */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    /* Setting up the oscillator at port 4, pin 1 */
    CAPTIO0CTL |= (1 << 8); // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4; // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1; // Choose Pin 1

    /* Setting up Timer_A module */
    Timer_A_ContinuousModeConfig timer_continuous_obj;  // Declare Timer_A object
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK; //INCLK
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;    //No division for high precision
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;    //Disable interrupt
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR; //Clear timer and disable during configuration
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj); //Configure mode
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);   //Start the timer

    while(1){
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);  //Clear timer to count up from 0
        delay_ms(2);   //Timer delay for frequency
        timer_A_cycles = MAP_Timer_A_getCounterValue(TIMER_A2_BASE); //Read the counter register
        delay_ms(500);  //Print delay for console output
        printf("Frequency: %dHz\n", (int)(timer_A_cycles*500));    //Frequency of Timer_A
    }
}

/* Delay number of milliseconds */
void delay_ms(uint32_t count)
{
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);                //Get current count
    cycles = count * (MAP_CS_getMCLK()/1000);   //Calculate number of cycles

    do{

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

    }while((t0 - t1) < cycles);                 //Check cycle count

    return;
}
