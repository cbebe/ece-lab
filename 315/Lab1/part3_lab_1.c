/**
 * ECE-315 WINTER 2021 - COMPUTER INTERFACING COURSE
 *
 * Created on: 15 July, 2021
 * Author: Shyama M. Gandhi, Mazen Elbaz
 * Modified by: Charles Ancheta, Pushkar Sabharwal
 *
 * IMPLEMENTATION OF A SIMPLE CALCULATOR FOR PART-3 LAB-1
 */

/* Include FreeRTOS Library */
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

/* GPIO Variable for the RGB LED */
XGpio RGBInst;

#define STACK_SIZE configMINIMAL_STACK_SIZE
#define DEFAULT_KEYTABLE "0FED789C456B123A"

/* GPIO Parameter definitions from xparameters.h */
#define RGBLED_DEVICE_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID

#define WHITE_IN_RGB 7

/* Key values of each operation */
#define ADD 'A'
#define SUB 'B'
#define MUL 'C'
#define PAL 'D'
/* Check if key value is an operator */
#define isOperator(key) ((key) == ADD || (key) == SUB || (key) == MUL || (key) == PAL)

/**
 * @brief Macros to print the result of an operation
 *
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros 😉
 */
#define printResult(operator, operands)                                                            \
    xil_printf("The operation %u %s %u = %u\r\n", operands[0], #operator, operands[1],             \
               operands[0] operator operands[1])
#define printOverflow(operator, operands)                                                          \
    xil_printf("The operation %u %s %u results in an overflow!\r\n", operands[0], #operator,       \
               operands[1])

/**
 * @brief Macro to check for an overflow before doing a binary operation
 *
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros 😉
 *
 * @param operator the symbol of the binary operator (+, -, *)
 * @param operands an array containing two operands
 * @param isOverflow function to check for an overflow in the operands
 */
#define doBinaryOperation(operator, operands, isOverflow)                                          \
    do {                                                                                           \
        if (isOverflow(operands))                                                                  \
            printOverflow(operator, operands);                                                     \
        else                                                                                       \
            printResult(operator, operands);                                                       \
    } while (0)

/* MAIN FUNCTION */
int main(void) {
    int status;

    xil_printf("System Ready!\n");

    /* Initialize RGB LED */
    status = XGpio_Initialize(&RGBInst, RGBLED_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization for RGB LED unsuccessful.\r\n");
        return XST_FAILURE;
    }

    /* Set RGB LED direction to output */
    XGpio_SetDataDirection(&RGBInst, 1, 0x00);

    xTaskCreate(prvTxTask, (const char *)"Tx", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &xTxTask);
    xTaskCreate(prvRxTask, (const char *)"Rx", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &xRxTask);

    /* The queue holds the two operands and the key value of the operator */
    xQueue = xQueueCreate(3, sizeof(unsigned int));
    configASSERT(xQueue);

    /* Initialize keypad */
    KYPD_begin(&myDevice, XPAR_AXI_GPIO_PMOD_KEYPAD_BASEADDR);
    KYPD_loadKeyTable(&myDevice, (u8 *)DEFAULT_KEYTABLE);

    vTaskStartScheduler();

    while (1)
        ;

    return 0;
}

/**
 * @brief Aggregate the current value as the user inputs more digits
 *
 * @param currentValue
 * @param key
 * @return u32 the result of the digit aggregation, 0 if the value overflowed
 */
static inline u32 aggregateValue(u32 currentValue, u8 key) {
    u32 factor = (int)(key - '0');
    u32 new_value = currentValue * 10 + factor;
    xil_printf("current_value = %u\n", new_value);
    /* Handle operand overflow here before sending to the queue */
    if (new_value % 10 != factor || new_value / 10 != currentValue) {
        xil_printf("Overflow detected! Resetting input...\r\n");
        new_value = 0;
        xil_printf("current_value = %u\n", new_value);
    }
    return new_value;
}

/**
 * @brief Register the aggregated number as an operand
 *
 * @param currentValuePtr pointer to the currentValue
 */
static inline void registerOperand(u32 *currentValuePtr) {
    const char *prompt = "Please select an operation.\r\n";
    /* Reject further input when queue is full */
    if (uxQueueMessagesWaiting(xQueue) == 2)
        xil_printf("2 operands are already in the queue. %s", prompt);
    else {
        xil_printf("Final current_value of operand= %u\n", *currentValuePtr);
        xQueueSendToBack(xQueue, currentValuePtr, 0UL);
        *currentValuePtr = 0;
        if (uxQueueMessagesWaiting(xQueue) == 2) xil_printf("%s", prompt);
    }
}

/**
 * @brief Handle the pressed key
 *
 * @param key value of the pressed key
 * @param currentValuePtr pointer to the aggregated value from the pressed digits
 */
static inline void handleKey(u8 key, u32 *currentValuePtr) {
    if ((char)key == 'F') registerOperand(currentValuePtr);
    /* Reset the current value of the operand and allow the user to enter a new value */
    else if ((char)key == 'E') {
        *currentValuePtr = 0;
        xil_printf("current_value of operand has been reset. "
                   "Please enter the new value.\n");
    } else if (key >= '0' && key <= '9')
        *currentValuePtr = aggregateValue(*currentValuePtr, key);
    /* Send the operator to the queue for the Rx task to process */
    else if ((uxQueueMessagesWaiting(xQueue) == 2) && isOperator((char)key)) {
        *currentValuePtr = (u32)key;
        xQueueSendToBack(xQueue, currentValuePtr, 0UL);
        *currentValuePtr = 0;
    }
}

static void prvTxTask(void *pvParameters) {
    UBaseType_t uxPriority;

    for (;;) {
        XStatus status, last_status = KYPD_NO_KEY;
        u8 key, last_key = 'x';
        u32 current_value = 0;

        Xil_Out32(myDevice.GPIO_addr, 0xF);
        xil_printf("PMOD KYPD demo started. Press any key on the Keypad.\r\n");

        uxPriority = uxTaskPriorityGet(NULL);

        while (1) {
            /* Capture state of each key and determine which single key is pressed, if any */
            status = KYPD_getKeyPressed(&myDevice, KYPD_getKeyStates(&myDevice), &key);
            /* Dynamically change priority to start Rx task once the queue is full */
            if (uxQueueMessagesWaiting(xQueue) == 3) vTaskPrioritySet(NULL, uxPriority - 2);
            /* Print key detect if a new key is pressed or if status has changed */
            if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key)) {
                xil_printf("Key Pressed: %c\r\n", (char)key);
                last_key = key;
                handleKey(key, &current_value);
            }
            /* this is valid whenever two or more keys are pressed together */
            else if (status == KYPD_MULTI_KEY && status != last_status)
                xil_printf("Error: Multiple keys pressed\r\n");

            last_status = status;
            usleep(1000);
        }
    }
}

/**
 * @brief Checks if the operands are palindromes, blinks a white light if both operands are
 *
 * @param operands array of operands
 */
static void checkPalindromes(u32 operands[]);

/* Functions to check for various types of overflow */
static u8 isAddOverflow(u32 ops[]) { return (ops[1] > (UINT32_MAX - ops[0])); }
static u8 isSubOverflow(u32 ops[]) { return ops[0] < ops[1]; }
static u8 isMulOverflow(u32 ops[]) { return (ops[0] * ops[1]) / ops[0] != ops[1]; }

static void prvRxTask(void *pvParameters) {
    UBaseType_t uxPriority;
    uxPriority = uxTaskPriorityGet(NULL);

    for (;;) {
        u32 operands[2], operator;
        /* Receive operands and the operator from the queue */
        xQueueReceive(xQueue, operands, 0UL);
        xQueueReceive(xQueue, operands + 1, 0UL);
        xQueueReceive(xQueue, &operator, 0UL);
        switch ((char)operator) {
            /* clang-format off */
        case ADD: doBinaryOperation(+, operands, isAddOverflow); break;
        case SUB: doBinaryOperation(-, operands, isSubOverflow); break;
        case MUL: doBinaryOperation(*, operands, isMulOverflow); break;
        case PAL: checkPalindromes(operands); break;
        default: xil_printf("Invalid operation!\r\n"); /* Something went wrong! */
            /* clang-format on */
        }
        xil_printf("Operation done.\r\n");

        /* Dynamically change Tx task priority to restart Tx task */
        vTaskPrioritySet(xTxTask, (uxPriority + 1));
    }
}

/**
 * @brief Checks if a number is a palindrome
 *
 * @param number number to check
 * @return u8 1 if the number is a palindrome, 0 if not
 */
static u8 isPalindrome(u32 number) {
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
    u8 isFirstPalindrome = isPalindrome(operands[0]);
    u8 isSecondPalindrome = isPalindrome(operands[1]);
    if (isFirstPalindrome && isSecondPalindrome) {
        xil_printf("%u and %u are both palindromes! Here's a blinding white light.\r\n",
                   operands[0], operands[1]);
        XGpio_DiscreteWrite(&RGBInst, 1, WHITE_IN_RGB);
        vTaskDelay(xDelay1500ms);
        XGpio_DiscreteWrite(&RGBInst, 1, 0x00);
    }
}
