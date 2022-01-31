/*
    Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
    Copyright (C) 2012 - 2018 Xilinx, Inc. All Rights Reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software. If you wish to use our
   Amazon FreeRTOS name, please do so in a fair use way that does not cause
   confusion.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

    http://www.FreeRTOS.org
    http://aws.amazon.com/freertos

    1 tab == 4 spaces!

     * ECE - 315 	: Computer Interfacing
     * Tutorial Lab
     * Created On	: July 7, 2021
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"

#define TIMER_ID 1
#define DELAY_5_SECONDS 5000UL
#define DELAY_1_SECOND 1000UL
#define TIMER_CHECK_THRESHOLD 1
/*-----------------------------------------------------------*/

/* The Tx task that will print the string="hello world" at interval of 1
 * second*/
static void prvTxTask(void *pvParameters);
static void vTimerCallback(TimerHandle_t pxTimer);
/*-----------------------------------------------------------*/

/*
 * Task and Timer Handler for TxTask and Timer Task
 */
static TaskHandle_t xTxTask;
static TimerHandle_t xTimer = NULL;
char HWstring[15] = "Hello World";

long TxtaskCntr = 0;

int main(void) {
    const TickType_t x5seconds = pdMS_TO_TICKS(DELAY_5_SECONDS);

    xil_printf("*** We are here in the main function, HELLO! ***\r\n");

    /* Create the task. */
    xTaskCreate(prvTxTask,          /* The function that implements the task. */
                (const char *)"Tx", /* Text name for the task, provided to
                                       assist debugging only. */
                configMINIMAL_STACK_SIZE, /* The stack allocated to the task. */
                NULL, /* The task parameter is not used, so set to NULL. */
                tskIDLE_PRIORITY, /* The task runs at the idle priority. */
                &xTxTask);

    // The timer expiry is set to 5 seconds and the timer set to not auto
    // reload.

    xTimer = xTimerCreate((const char *)"Timer", x5seconds, pdFALSE,
                          (void *)TIMER_ID, vTimerCallback);
    /* Check the timer was created. */
    configASSERT(xTimer);

    /* start the timer with a block time of 0 ticks. This means as soon
       as the schedule starts the timer will start running and will expire after
       5 seconds */
    xTimerStart(xTimer, 0);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    for (;;)
        ;
}

/*-----------------------------------------------------------*/
static void prvTxTask(void *pvParameters) {
    const TickType_t x1second = pdMS_TO_TICKS(DELAY_1_SECOND);

    for (;;) {
        /* Delay for 1 second. */
        vTaskDelay(x1second);

        /*******************************************************************************************
         * STUDENTS NEED TO MODIFY THE FOLLOWING 'xil_printf' LINE FOR PART-1 OF
         *THE TUTORIAL LAB
         *******************************************************************************************/
        xil_printf("\n ...Hello World... \n");

        TxtaskCntr++;
    }
}

/*-----------------------------------------------------------*/
static void vTimerCallback(TimerHandle_t pxTimer) {
    long lTimerId;
    configASSERT(pxTimer);

    lTimerId = (long)pvTimerGetTimerID(pxTimer);

    if (lTimerId != TIMER_ID) {
        xil_printf("\nFreeRTOS Hello World Example FAILED\n");
    }

    /* If the TxtaskCntr is updated every time the Tx task is called. The Tx
     task prints a message every 1 second. The timer expires after 5 seconds. */
    if (TxtaskCntr >= TIMER_CHECK_THRESHOLD) {
        xil_printf("\nTxtaskCntr = %d\n", TxtaskCntr);
        xil_printf("\nFreeRTOS Hello World Example PASSED\n");
    } else {
        xil_printf("\nFreeRTOS Hello World Example FAILED\n");
    }

    vTaskDelete(xTxTask); // delete the Tx task
}
