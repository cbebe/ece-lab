/*
 *  ECE- 315 WINTER 2021 - COMPUTER INTERFACING COURSE
 *
 *  Created on	: 	15 July, 2021
 *  Author		: 	Shyama M. Gandhi, Mazen Elbaz
 *
 * -----------------------------------------------------------------------------------------------------
 * IMPLEMENTATION OF A SIMPLE CALCULATOR.
 * Inputs Operands from the keypad
 * Output of the arithmetic operation is displayed on the Console
 * This exercise of the lab does not use SSD!!!
 * Operations available : +, -, * and palindrome, selected using the keys A, B,
 * C and D, respectively.
 *
 * The design of this exercise is as follows:
 * Say, you wish to calculate (978 X 4050)
 * So, you enter 9, press 7, press 8 and then F so that the operand will be
 * registered. Do the same for second operand of 4050. In case while entering
 * the operand, if you commit any error, you can press 'E' key any time to enter
 * the operand again. Once you have entered two operands, press any key from A,
 * B, C or D to choose the corresponding operation. So, the sequence of inputs
 * you entered is, enter one operand, enter second operand and then enter the
 * operation using the (A/B/C/D) key. The calculator is designed in a way that
 * you enter the operands first and then select the operation as a third value
 * to the Queue. The corresponding output will be displayed on the console.
 * 32-bit variables are used to store the input as well as output and overflow
 * will generate a wrong output. You must detect the overflow condition for +, -
 * and * operation.
 *
 * For subtraction, you may use store_operands[1]-store_operands[0] or vice
 * versa. For palindrome, two operands are taken as an input. Four results are
 * possible: both operands are palindrome, operand 1 is palindrome but operand 2
 * is not. Operand 2 is palindrome but operand 1 is not. Both operands are
 * non-palindrome numbers. For any case, display the result on the console.
 * However, for the case when both the operands are palindrome, a White light on
 * RGB led must glow for 1.5 seconds. The initialization and required
 * definitions for RGB led GPIO has been provided as you read along the
 * template.
 * -----------------------------------------------------------------------------------------------------
 *
 */

// Include FreeRTOS Library
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "xgpio.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"

#include "math.h"
#include "pmodkypd.h"
#include "sleep.h"
#include "xil_cache.h"

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask(void *pvParameters);
static void prvRxTask(void *pvParameters);

u32 SSD_decode(u8 key_value, u8 cathode);

PmodKYPD myDevice;

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;

// GPIO Variable RGBLED
XGpio RGBInst;

#define DEFAULT_KEYTABLE "0FED789C456B123A"

// GPIO Parameter definitions from xparameters.h
#define RGBLED_DEVICE_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID

#define WHITE_IN_RGB 7

#define ADD 'A'
#define SUB 'B'
#define MUL 'C'
#define PAL 'D'

// Non-syntactic macros
#define doOperation(operation, operands)                                                           \
    xil_printf("The operation %u %s %u = %u\r\n", operands[0], #operation, operands[1],            \
               operands[0] operation operands[1])
#define printOverflow(operation, operands)                                                         \
    xil_printf("The operation %u %s %u results in an overflow!\r\n", operands[0], #operation,      \
               operands[1])

// MAIN FUNCTION
int main(void) {
    int status;

    xil_printf("System Ready!\n");

    // Initialize RGB LED
    status = XGpio_Initialize(&RGBInst, RGBLED_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization for RGB LED unsuccessful.\r\n");
        return XST_FAILURE;
    }

    // Set RGB LED direction to output
    XGpio_SetDataDirection(&RGBInst, 1, 0x00);

    /* Create the two tasks.  The Tx task is given a higher priority than the Rx task. Dynamically
     * changing the priority of Rx Task later on so the Rx task will leave the Blocked state and
     * pre-empt the Tx task as soon as the Tx task fills the queue. */
    xTaskCreate(prvTxTask, (const char *)"Tx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2,
                &xTxTask);
    xTaskCreate(prvRxTask, (const char *)"Rx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1,
                &xRxTask);

    /* Create the queue used by the tasks.
     * There are three items in the queue, two operands and then operation using keypad
     * Each space in the queue is large enough to hold a uint32_t.
     */
    xQueue = xQueueCreate(3, sizeof(unsigned int));

    /* Check the queue was created. */
    configASSERT(xQueue);

    // Initialize keypad
    KYPD_begin(&myDevice, XPAR_AXI_GPIO_PMOD_KEYPAD_BASEADDR);
    KYPD_loadKeyTable(&myDevice, (u8 *)DEFAULT_KEYTABLE);

    vTaskStartScheduler();

    while (1)
        ;

    return 0;
}

/*-----------------------------------------------------------*/
static void prvTxTask(void *pvParameters) {
    UBaseType_t uxPriority;

    for (;;) {
        XStatus status, last_status = KYPD_NO_KEY;
        u8 key, last_key = 'x';
        u32 factor = 0, current_value = 0, old_value;

        Xil_Out32(myDevice.GPIO_addr, 0xF);
        xil_printf("PMOD KYPD demo started. Press any key on the Keypad.\r\n");

        uxPriority = uxTaskPriorityGet(NULL);

        while (1) {

            // Capture state of each key and determine which single key is pressed, if any
            status = KYPD_getKeyPressed(&myDevice, KYPD_getKeyStates(&myDevice), &key);

            /*********************************/
            // enter the function to dynamically change the priority when queue is full.This way
            // when the queue is full here, we change the priority of this task and hence queue
            // will be read in the receive task to perform the operation.If you change the
            // priority here dynamically, make sure in the receive task to do the counter part
            // !!!
            /*********************************/
            if (uxQueueMessagesWaiting(xQueue) == 3) vTaskPrioritySet(NULL, uxPriority - 2);

            // Print key detect if a new key is pressed or if status has changed
            if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key)) {
                xil_printf("Key Pressed: %c\r\n", (char)key);
                last_key = key;

                // whenever 'F' is pressed, the aggregated number will be registered as an operand
                if ((char)key == 'F') {
                    /*******************************/
                    // write the logic to enter the updated variable here to the Queue
                    /*******************************/
                    if (uxQueueMessagesWaiting(xQueue) == 2)
                        xil_printf(
                            "2 operands are already in the queue. Please select an operation.\r\n");
                    else {
                        xil_printf("Final current_value of operand= %u\n", current_value);
                        xQueueSendToBack(xQueue, &current_value, 0UL);
                        current_value = 0;
                        if (uxQueueMessagesWaiting(xQueue) == 2)
                            xil_printf("Please select an operation.\r\n");
                    }
                }
                // if 'E' is pressed, it resets the current value of the operand and allows the user
                // to enter a new value
                else if ((char)key == 'E') {
                    xil_printf("current_value of operand has been reset. "
                               "Please enter the new value.\n");
                    factor = current_value = 0;
                }
                // case when we consider input key strokes from '0' to '9' (only these are the valid
                // key inputs for all the four operations) the current_value is aggregated as the
                // user presses consecutive digits e.g. if the user presses the following digits in
                // this order 4 > 5 > 8  => current_value will end up being 458
                else if (key >= '0' && key <= '9') {
                    factor = (int)(key - '0');
                    old_value = current_value;
                    current_value = current_value * 10 + factor;
                    xil_printf("current_value = %u\n", current_value);
                    // Handle overflow here before sending to the queue
                    if (current_value % 10 != factor || current_value / 10 != old_value) {
                        xil_printf("Overflow detected! Resetting input...\r\n");
                        factor = current_value = 0;
                        xil_printf("current_value = %u\n", current_value);
                    }
                } else if ((uxQueueMessagesWaiting(xQueue) == 2) &&
                           ((char)key == ADD || (char)key == SUB || (char)key == MUL ||
                            (char)key == PAL)) {
                    /*****************************************/
                    // once two operands are in the queue, enter the third value to the queue to
                    // indicate the operation to be performed using A,B,C or D key store the current
                    // key value to the queue as the third element
                    /*****************************************/
                    current_value = (u32)key;
                    xQueueSendToBack(xQueue, &current_value, 0UL);
                    current_value = 0;
                }
            }
            // this is valid whenever two or more keys are pressed together
            else if (status == KYPD_MULTI_KEY && status != last_status)
                xil_printf("Error: Multiple keys pressed\r\n");

            last_status = status;
            usleep(1000);
        }
    }
}

// Valid operations
static void checkPalindromes(u32 operands[]);
static void doAddition(u32 operands[]);
static void doSubtraction(u32 operands[]);
static void doMultiplication(u32 operands[]);

/*-----------------------------------------------------------*/
static void prvRxTask(void *pvParameters) {
    UBaseType_t uxPriority;
    uxPriority = uxTaskPriorityGet(NULL);

    for (;;) {
        u32 operands[2], operator;
        /***************************************/
        // ...Write code here to read the three elements from the queue and perform the required
        // operation.
        // ...Display the output result on the console for all the four operations.
        // ...If you have dynamically changed the priority of this task in TxTask, you need to
        // change the priority here accordingly, respectively using vTaskPrioritySet(). This can be
        // done after you finish calculation part.
        // ...This way once the RxTask is done, TxTask will have a higher priority and hence will
        // wait for the next series of inputs from the user
        // ...You can write a switch-case statement or if-else statements for each different
        // operation
        //...For the Palindrome check, think of a way to find the reverse of each operand (two loops
        // for each operand!) Compared this reverse operand with the original operand.
        // ...For RGB led, look at the function that was used in previous labs for writing the value
        // to the led. Initialization and color definition is already provided to you in this file.
        /***************************************/
        xQueueReceive(xQueue, operands, 0UL);
        xQueueReceive(xQueue, operands + 1, 0UL);
        xQueueReceive(xQueue, &operator, 0UL);
        switch ((char)operator) {
            /* clang-format off */
        case ADD: doAddition(operands); break;
        case SUB: doSubtraction(operands); break;
        case MUL: doMultiplication(operands); break;
        case PAL: checkPalindromes(operands); break;
        default: xil_printf("Invalid operation!\r\n"); // Something went wrong!
            /* clang-format on */
        }
        xil_printf("Operation done.\r\n");
        vTaskPrioritySet(xTxTask, (uxPriority + 1));
    }
}

static u32 isPalindrome(u32 number) {
    u32 value = 0, factor = number;
    while (factor > 0) {
        value = value * 10 + (factor % 10);
        factor /= 10;
    }
    xil_printf("%u is ", number);
    if (value != number) xil_printf("not ");
    xil_printf("a palindrome!\r\n");
    return value == number;
}

static void checkPalindromes(u32 operands[]) {
    const TickType_t xDelay1500ms = pdMS_TO_TICKS(1500UL);
    if (isPalindrome(operands[0]) && isPalindrome(operands[1])) {
        xil_printf("%u and %u are both palindromes! Here's a blinding white light.\r\n",
                   operands[0], operands[1]);
        XGpio_DiscreteWrite(&RGBInst, 1, 0x07);
        vTaskDelay(xDelay1500ms);
        XGpio_DiscreteWrite(&RGBInst, 1, 0x00);
    }
}

static void doAddition(u32 operands[]) {
    if (operands[0] > 0 && operands[1] > (UINT32_MAX - operands[0]))
        printOverflow(+, operands);
    else
        doOperation(+, operands);
}

static void doSubtraction(u32 operands[]) {
    if (operands[0] < operands[1])
        printOverflow(-, operands);
    else
        doOperation(-, operands);
}

static void doMultiplication(u32 operands[]) {
    u32 result = operands[0] * operands[1];
    if (result / operands[0] != operands[1])
        printOverflow(*, operands);
    else
        doOperation(*, operands);
}

