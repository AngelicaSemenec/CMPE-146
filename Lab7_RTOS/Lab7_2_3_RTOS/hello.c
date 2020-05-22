/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 */

/*
 *  ======== hello.c ========
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#define TASKSTACKSIZE   2048

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);

Task_Struct task1Struct, task2Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];
Semaphore_Struct semStruct;
Semaphore_Handle semHandle;

int main()
{
    //NOTE: Code for setting up mutex taken from SDK example code
    //mutex_MSP_EXP432P401R_tirtos_ccs

    /* Construct BIOS objects */
    Task_Params taskParams;
    Semaphore_Params semParams;

    /* Call driver init functions */
    Board_init();

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);

    /* Obtain instance handle */
    semHandle = Semaphore_handle(&semStruct);

    BIOS_start();    /* Does not return */
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{
    //Button S1
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    printf("Task1\n");
    UART_Handle uart;
    UART_Params uartParams;

    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;

    char buffer[10];
    time_t t0;
    uint8_t state = 1;
    while (1) {
        t0 = time(NULL);
        while (time(NULL) - t0 < 5 && state == MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1))
        {
            Task_yield();
        }
        state = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);

        /* Get access to resource */
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);

        uart = UART_open(CONFIG_UART_0, &uartParams);
        sprintf(buffer, "S1%d ", state);
        UART_write(uart, buffer, strlen(buffer));
        UART_close(uart);

        /* Unlock resource */
        Semaphore_post(semHandle);
    }
}

Void task2(UArg arg0, UArg arg1)
{
    //Button S2
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

    printf("Task2\n");
    UART_Handle uart;
    UART_Params uartParams;

    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;

    char buffer[10];
    time_t t0;
    uint8_t state = 1;
    while (1) {
        t0 = time(NULL);
        while (time(NULL) - t0 < 5 && state == MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4))
        {
            Task_yield();
        }
        state = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);

        /* Get access to resource */
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);

        uart = UART_open(CONFIG_UART_0, &uartParams);
        sprintf(buffer, "S2%d ", state);
        UART_write(uart, buffer, strlen(buffer));
        UART_close(uart);

        /* Unlock resource */
        Semaphore_post(semHandle);
    }
}
