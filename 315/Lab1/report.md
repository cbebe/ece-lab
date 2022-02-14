---
title: ECE 315 Lab 1 Report
subtitle: Winter 2022
fontsize: 12pt
geometry: margin=0.75in
author: Charles Ancheta (1581672), Pushkar Sabharwal (1588927)
---

# Objectives

The main objective of this lab is to gain experience in designing tasks, delays and queues in freeRTOS. The lab also involves interfacing with inputs received from a keypad and push-buttons and interfacing to outputs using a 7-segment LED display. We use all this knowledge obtained to design a full fledged calculator. This lab is divided into three parts to achieve all the operations and make progress simpler.

# Exercise 1: Lighting up two LED digits from the key press

Part 1 of the lab involves displaying the key pressed hex digit to the right-side LED digit. Pressing another digit results in the new digit being shown on the right SSD and the previous digit is shown to the left.

```c
/**
 * ECE-315 WINTER 2021 - COMPUTER INTERFACING COURSE
 *
 * Created on: 14 July, 2021
 * Author: Shyama M. Gandhi, Mazen Elbaz
 * Modified by: Charles Ancheta, Pushkar Sabharwal
 *
 * KEYPAD AND SEVEN-SEGMENT DECODER IMPLEMENTATION FOR PART-1 LAB-1
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

#include "pmodkypd.h" /* PMOD keypad driver provided by digilent */
#include "sleep.h"
#include "xil_cache.h"

/* Parameter definitions */
#define SSD_DEVICE_ID XPAR_AXI_GPIO_PMOD_SSD_DEVICE_ID
#define KYPD_DEVICE_ID XPAR_AXI_GPIO_PMOD_KEYPAD_DEVICE_ID

/* The maximum delay that prevents the SSD from flickering */
#define SSD_MS_DELAY 15

/* The cathode value for the right seven-segment digit */
#define RIGHT_SSD 0
/* The cathode value for the left seven-segment digit */
#define LEFT_SSD 1
/* Direction bitmask to set all the pins to be output */
#define OUTPUT_DIRECTION_MASK 0x00

/**
 * keytable is determined as follows (indices shown in Keypad position below)
 * 12 13 14 15
 * 8  9  10 11
 * 4  5  6  7
 * 0  1  2  3
 */
#define DEFAULT_KEYTABLE "0FED789C456B123A"

/* Button Variable */
XGpio SSDInst, KYPDInst;

/* The Tx described at the top of this file. */
static void prvTxTask(void *pvParameters);

/* Provided function to decode the key value for the seven-segment digit */
static u32 SSD_decode(u8 key_value, u8 cathode);

/**
 * @brief Decode the keypad code and display the key into the given SSD
 * Also adds the delay SSD_MS_DELAY
 *
 * @param key the ASCII value of the key to be displayed
 * @param ssd the cathode value to be used on the seven segment digit
 */
static inline void showDigit(u8 key, u8 ssd);

PmodKYPD myDevice;

static TaskHandle_t xTxTask;

/* MAIN FUNCTION */
int main(void) {
    int status;

    /* Initialize SSD */
    status = XGpio_Initialize(&SSDInst, SSD_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization for SSD unsuccessful.\r\n");
        return XST_FAILURE;
    }
    XGpio_SetDataDirection(&SSDInst, 1, OUTPUT_DIRECTION_MASK);

    xil_printf("Initialization Complete, System Ready!\n");

    xTaskCreate(prvTxTask, (const char *)"Tx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY,
                &xTxTask);

    /* Initialize keypad with keytable */
    KYPD_begin(&myDevice, XPAR_AXI_GPIO_PMOD_KEYPAD_BASEADDR);
    KYPD_loadKeyTable(&myDevice, (u8 *)DEFAULT_KEYTABLE);

    vTaskStartScheduler();

    for (;;)
        ;

    return 0;
}

static void prvTxTask(void *pvParameters) {
    for (;;) {
        u16 keystate;
        XStatus status, last_status = KYPD_NO_KEY;
        u8 new_key, current_key = 'x', previous_key = 'x';

        /* Initial value of last_key cannot be contained in loaded KEYTABLE string */
        Xil_Out32(myDevice.GPIO_addr, 0xF);

        xil_printf("Pmod KYPD demo started. Press any key on the Keypad.\r\n");
        for (;;) {
            /* Capture state of each key */
            keystate = KYPD_getKeyStates(&myDevice);

            /* Determine which single key is pressed, if any */
            /* if a key is pressed, stored the value of the new key in new_key */
            status = KYPD_getKeyPressed(&myDevice, keystate, &new_key);

            /* Print key detect if a new key is pressed or if status has changed */
            if (status == KYPD_SINGLE_KEY && (status != last_status || new_key != current_key)) {
                xil_printf("Key Pressed: %c\r\n", (char)new_key);
                previous_key = current_key;
                current_key = new_key;
            } else if (status == KYPD_MULTI_KEY && status != last_status)
                xil_printf("Error: Multiple keys pressed\r\n");

            last_status = status;

            /* Clear the left SSD first */
            XGpio_DiscreteWrite(&SSDInst, 1, 0b10000000);
            showDigit(current_key, RIGHT_SSD);
            showDigit(previous_key, LEFT_SSD);
        }
    }
}

static u32 SSD_decode(u8 key_value, u8 cathode) {
    /* clang-format off */
    switch(key_value) {
        case '0': if (cathode == 0) return 0b00111111; else return 0b10111111;
        case '1': if (cathode == 0) return 0b00000110; else return 0b10000110;
        case '2': if (cathode == 0) return 0b01011011; else return 0b11011011;
        case '3': if (cathode == 0) return 0b01001111; else return 0b11001111;
        case '4': if (cathode == 0) return 0b01100110; else return 0b11100110;
        case '5': if (cathode == 0) return 0b01101101; else return 0b11101101;
        case '6': if (cathode == 0) return 0b01111101; else return 0b11111101;
        case '7': if (cathode == 0) return 0b00000111; else return 0b10000111;
        case '8': if (cathode == 0) return 0b01111111; else return 0b11111111;
        case '9': if (cathode == 0) return 0b01101111; else return 0b11101111;
        case 'A': if (cathode == 0) return 0b01110111; else return 0b11110111;
        case 'B': if (cathode == 0) return 0b01111100; else return 0b11111100;
        case 'C': if (cathode == 0) return 0b00111001; else return 0b10111001;
        case 'D': if (cathode == 0) return 0b01011110; else return 0b11011110;
        case 'E': if (cathode == 0) return 0b01111001; else return 0b11111001;
        case 'F': if (cathode == 0) return 0b01110001; else return 0b11110001;
        default:  if (cathode == 0) return 0b00000000; else return 0b00000000;
    }
    /* clang-format on */
}

static inline void showDigit(u8 key, u8 ssd) {
    XGpio_DiscreteWrite(&SSDInst, 1, SSD_decode(key, ssd));
    vTaskDelay(pdMS_TO_TICKS(SSD_MS_DELAY));
}
```

First, we define the global variables to complete the execution of this part, for this part we define the delay that prevents the SSD from flickering to be 15. Then we define the right part of the seven-segment display as 0, and left part of the seven-segment display as 1, we also define output direction mask as 0x00. Then we just need to modify the SSD_Decode() function by adding more switch cases. We check for the value 'key_value' from '0'-'F' in hexadecimal value and for each value, and define an 8 bit binary value for each one. For instance, for the case '0', we first check if the value of the 'cathode' value is 0, if it's 0, then we return 0b00111111 otherwise, we define it as 0b10111111.

# Exercise 2: Entering multiple digits using the keypad to perform logical and modulo operations

In part 2 of the lab, we are required to implement a system in which, when a keypad key corresponding to a decimal digit is pressed, the corresponding digit will be displayed on the sdk terminal. Inputs from the keys A, B, C, D and F are to be ignored. The program is then required to interpret the two inputted digits as operands and give the ability to perform various functions like AND, OR, XOR and Modulo function using the operands.

```c
/*
 * ECE-315 WINTER 2021 - COMPUTER INTERFACING COURSE
 *
 * Created on: 14 July, 2021
 * Author: Shyama M. Gandhi, Mazen Elbaz
 * Modified by: Charles Ancheta, Pushkar Sabharwal
 *
 * IMPLEMENTATION OF A SINGLE DIGIT LOGICAL AND MODULO CALCULATOR FOR PART-2 LAB-1
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

/* Parameter definitions */
#define BTNS_DEVICE_ID XPAR_AXI_GPIO_BUTTONS_DEVICE_ID
#define SSD_DEVICE_ID XPAR_AXI_GPIO_PMOD_SSD_DEVICE_ID

#define STACK_SIZE configMINIMAL_STACK_SIZE

/* Button Variable */
XGpio BTNInst, SSDInst;

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask(void *pvParameters);
static void prvRxTask(void *pvParameters);
static u32 SSD_decode(u8 key_value, u8 cathode);

PmodKYPD myDevice;

/*-----------------------------------------------------------*/

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;

#define DEFAULT_KEYTABLE "0FED789C456B123A"

#define OUTPUT_DIRECTION_MASK 0x00
#define INPUT_DIRECTION_MASK 0xFF
#define NEGATIVE_SIGN 0xFF

/* The maximum delay that prevents the SSD from flickering */
#define SSD_MS_DELAY 15
#define RIGHT_SSD 0
#define LEFT_SSD 1

/* Button values for different operations */
#define XOR 1
#define OR 2
#define AND 4
#define MOD 8

/**
 * @brief Macro to initialize a GPIO pin
 *
 * @param ptr pointer to the device instance
 * @param deviceID ID of the device as defined in xparameters.h
 * @param name name of the device for debugging purposes
 * @param direction direction bitmask for the pin
 */
#define initializeGPIO(ptr, deviceID, name, direction)                                             \
    do {                                                                                           \
        if (XGpio_Initialize(ptr, deviceID) != XST_SUCCESS) {                                      \
            xil_printf("GPIO Initialization for " name " unsuccessful.\r\n");                      \
            return XST_FAILURE;                                                                    \
        }                                                                                          \
        XGpio_SetDataDirection(ptr, 1, direction);                                                 \
    } while (0)

/**
 * @brief Macro to do a single binary operation
 *
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros
 *
 * @param operator the symbol of the binary operator (^, |, &, %)
 * @param operands an array containing two operands
 * @param result variable in which the operation result is stored
 */
#define doOperation(operator, operands, result)                                                    \
    do {                                                                                           \
        result = operands[0] operator operands[1];                                                 \
        xil_printf("Operation %d %s %d = %d\r\n", operands[0], #operator, operands[1], result);    \
    } while (0)

/* MAIN FUNCTION */
int main(void) {
    initializeGPIO(&BTNInst, BTNS_DEVICE_ID, "BUTTONS", INPUT_DIRECTION_MASK);
    initializeGPIO(&SSDInst, SSD_DEVICE_ID, "SSD", OUTPUT_DIRECTION_MASK);

    xil_printf("Initialization Complete, System Ready!\n");

    xTaskCreate(prvTxTask, (const char *)"Tx", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &xTxTask);
    xTaskCreate(prvRxTask, (const char *)"Rx", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &xRxTask);
    /* Only store the two u32 operands in the queue */
    xQueue = xQueueCreate(2, sizeof(unsigned int));

    /* Check the queue was created. */
    configASSERT(xQueue);

    /* Initialize keypad */
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
        u8 key, last_key = 'x', store_key = 'x';
        u32 key_stroke_on_SSD = 0;

        /* Initial value of last_key cannot be contained in loaded KEYTABLE string */
        Xil_Out32(myDevice.GPIO_addr, 0xF);
        xil_printf("PMOD KYPD demo started. Press any key on the Keypad.\r\n");
        uxPriority = uxTaskPriorityGet(NULL);

        while (1) {
            /* Dynamically change priority to start Rx task once the queue is full */
            if (uxQueueMessagesWaiting(xQueue) == 2) vTaskPrioritySet(NULL, uxPriority - 2);

            /* Determine which single key is pressed, if any */
            status = KYPD_getKeyPressed(&myDevice, KYPD_getKeyStates(&myDevice), &key);

            /* Print key detect if a new key is pressed or if status has changed */
            if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key)) {
                xil_printf("Key Pressed: %c\r\n", (char)key);
                last_key = key;

                /* Ignore 'A', 'B', 'C', 'D', and 'F' keys */
                if (((char)key >= 'A' && (char)key <= 'D') || (char)key == 'F')
                    xil_printf("Keys A, B, C, D, and F are ignored for this application.\n");
                /* case when we consider input key strokes from '0' to '9' */
                /* (only these are the valid key inputs for arithmetic operation) */
                else if ((char)key != 'E')
                    store_key = key;
                /* if user presses 'E' key, consider the last input key pressed as the operand */
                else if ((char)key == 'E')
                    if (store_key >= '0' && store_key <= '9') {
                        xil_printf("Storing the operand %c to Queue...\n", (char)store_key);
                        store_key -= '0'; /* turn into actual decimal value */
                        key_stroke_on_SSD = SSD_decode(store_key, 0);
                        /* display the digit on SSD stored as an operand. */
                        XGpio_DiscreteWrite(&SSDInst, 1, key_stroke_on_SSD);
                        /* Store the key value. They want a u32 so we have to send a u32 address */
                        key_stroke_on_SSD = store_key;
                        xQueueSendToBack(xQueue, &key_stroke_on_SSD, 0UL);
                    } else
                        xil_printf("Invalid operand! Please press a key from 0-9.\r\n");
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
 * @brief Display the digit into the given SSD
 * Also adds the delay SSD_MS_DELAY
 *
 * @param key the value of the digit to be displayed
 * @param ssd the cathode value to be used on the seven segment digit
 */
static inline void showDigit(u8 digit, u8 ssd) {
    XGpio_DiscreteWrite(&SSDInst, 1, SSD_decode(digit, ssd));
    vTaskDelay(pdMS_TO_TICKS(SSD_MS_DELAY));
}

/*-----------------------------------------------------------*/
static void prvRxTask(void *pvParameters) {
    UBaseType_t uxPriority = uxTaskPriorityGet(NULL);

    for (;;) {
        u32 operands[2];
        u32 btn_value;
        int result = 0, valid = 0, first = 1, modulo_error = 0;

        /* Reads the values from the queue and stores it in operands */
        xQueueReceive(xQueue, operands, 0UL);
        xQueueReceive(xQueue, operands + 1, 0UL);

        /* keep the button pressed for your choice of the arithmetic/logical operation */
        while (!valid) {
            /* Read button value to get the operation to be done */
            btn_value = XGpio_DiscreteRead(&BTNInst, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            switch (btn_value) {
            /* clang-format off */
            case XOR: doOperation(^, operands, result); valid = 1; break;
            case OR: doOperation(|, operands, result); valid = 1; break;
            case AND: doOperation(&, operands, result); valid = 1; break;
            /* clang-format on */
            case MOD:
                if (operands[1])
                    doOperation(%, operands, result);
                else {
                    xil_printf("Modulo division error!\r\n");
                    modulo_error = 1;
                }
                valid = 1;
                break;
            default:
                if (first) {
                    xil_printf("Invalid operator! Please select a valid operator by only pressing "
                               "one button at a time.\r\nWaiting for operation input...\r\n");
                    first = 0;
                }
            }
        }

        if (result < 0) xil_printf("Result is less than zero!!!\n");
        /* delay to differentiate the input operands and the result output */
        vTaskDelay(pdMS_TO_TICKS(1500));

        if (!modulo_error) {
            /* valid result */
            u8 lsb = result % 10;
            u8 msb = result / 10;
            for (int i = 0; i < 100; ++i) {
                /* Compute MSB and LSB digits for 2-digit output */
                showDigit(lsb, RIGHT_SSD);
                showDigit(msb, LEFT_SSD);
            }
        } else {
            /* Modulo error: display -1 */
            for (int i = 0; i < 100; ++i) {
                showDigit(1, RIGHT_SSD);
                showDigit(NEGATIVE_SIGN, LEFT_SSD);
            }
        }

        /* clear both the segments after the result is displayed. */
        XGpio_DiscreteWrite(&SSDInst, 1, 0b00000000);
        XGpio_DiscreteWrite(&SSDInst, 1, 0b10000000);

        /* Dynamically change Tx task priority to restart Tx task */
        vTaskPrioritySet(xTxTask, (uxPriority + 1));
    }
}

/* valid key press from 0-9 has already been encoded for you. */
static u32 SSD_decode(u8 key_value, u8 cathode) {
    /* clang-format off */
    switch (key_value) {
    case 0:  if (cathode == 0) return 0b00111111; else return 0b10111111;
    case 1:  if (cathode == 0) return 0b00000110; else return 0b10000110;
    case 2:  if (cathode == 0) return 0b01011011; else return 0b11011011;
    case 3:  if (cathode == 0) return 0b01001111; else return 0b11001111;
    case 4:  if (cathode == 0) return 0b01100110; else return 0b11100110;
    case 5:  if (cathode == 0) return 0b01101101; else return 0b11101101;
    case 6:  if (cathode == 0) return 0b01111101; else return 0b11111101;
    case 7:  if (cathode == 0) return 0b00000111; else return 0b10000111;
    case 8:  if (cathode == 0) return 0b01111111; else return 0b11111111;
    case 9:  if (cathode == 0) return 0b01101111; else return 0b11101111;
    case NEGATIVE_SIGN: return 0b11000000;
    default: if (cathode == 0) return 0b00000000; else return 0b00000000;
    }
    /* clang-format on */
}
```

# Exercise 3: A Simple calculator

In part 3 of the lab, our goal is to program a simple calculator which can perform four-functions. These operations include addition, subtraction, multiplication and the ability to check if the entered number is a palindrome which is selected using the keys ‘A’, ‘B’, ‘C’ and ‘D’ respectively. The answer of all the operations will be shown in the SDK terminal.

```c
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
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros
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
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros
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
```

We first implement 'prvRxTask()' function to read three elements from the queue using 'xQueueReceive()' function which are the two operands and the operator. Then we have switch case to check for the type of the operator. Then call the function 'doBinaryOperation' and pass the operands and the type of overflow to the function. In the case of checking the palindrome, we call the function 'checkPalindromes' and pass the operands to it. The 'doBinaryOperation' function is a while loop that checks for the overflow first. If no overflow exists then we print the result using the 'printResult'.