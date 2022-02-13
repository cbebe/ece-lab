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
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Macros.html#Non-syntactic_macros 😉
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
