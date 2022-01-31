/*
 * AXI GPIO Implementation using the RGB led and 4 White LEDs on Zynq-7000
 * board. The OS used is FreeRTOS.
 *
 * Created on: Date: 07 July, 2021
 * Author: Shyama M. Gandhi, Mazen Elbaz
 * ECE_315 LAB: Winter 2022
 * Tutorial Lab
 *
 * Change the color at the RGB LED using the definitions provided in this
 * code. You will find the function responsible for changing the RGB LED in the
 * respective function definitions. Comments have been added for better insight.
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "xgpio.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"

// GPIO Parameter definitions from xparameters.h
#define LEDS_DEVICE_ID XPAR_AXI_GPIO_4LEDS_DEVICE_ID
#define RGBLED_DEVICE_ID XPAR_AXI_GPIO_RGBLED_DEVICE_ID

// Different color definitions using RGB led on board
#define RED_IN_RGB 1
#define GREEN_IN_RGB 2
#define YELLOW_IN_RGB 3
#define BLUE_IN_RGB 4
#define PINK_IN_RGB 5
#define NAVY_BLUE_IN_RGB 6
#define WHITE_IN_RGB 7

// GPIO Variables for LED, RGBLED.
XGpio LEDInst, RGBInst;

// Task definitions for any two colors. Here, Blue and Yellow is used.
static void Blue_Task(void *pvParameters);
static void Yellow_Task(void *pvParameters);

const char *pcTextForTask1 = "Blue task is running";
const char *pcTextForTask2 = "Yellow task is running";

int main(void) {
    int status;

    //----------------------------------------------------
    // INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO (RGB LED, LEDs)
    //----------------------------------------------------
    // Initialize LEDs
    status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization for LEDS unsuccessful.\r\n");
        return XST_FAILURE;
    }

    // Initialize RGB LED
    status = XGpio_Initialize(&RGBInst, RGBLED_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization for RGB LED unsuccessful.\r\n");
        return XST_FAILURE;
    }

    // Set LEDs direction to outputs
    XGpio_SetDataDirection(&LEDInst, 1, 0x00);
    // Set RGB LED direction to output
    XGpio_SetDataDirection(&RGBInst, 1, 0x00);

    xil_printf(
        "Initialization done for RGB led and LEDs ::: System Ready\r\n\n");

    // Create the Tasks
    xTaskCreate(Blue_Task, "Blue Task", 1000, (void *)pcTextForTask1, 3, NULL);
    xTaskCreate(Yellow_Task, "Yellow Task", 1000, (void *)pcTextForTask2, 2,
                NULL);

    /* Start the scheduler to start the tasks executing. */
    vTaskStartScheduler();

    for (;;)
        ;

    return 0;
}

static void Blue_Task(void *pvParameters) {

    const TickType_t xDelay1500ms = pdMS_TO_TICKS(1500UL);

    for (;;) {

        // Write the bit value (color code) to the white leds
        // There are four white leds
        XGpio_DiscreteWrite(&LEDInst, 1, 0b0001);

        // Write the bit value (color code) to the RGB LED using the definitions
        // provided at the top of this file
        /***CHANGE THE RGB LED COLOR HERE***/
        XGpio_DiscreteWrite(&RGBInst, 1, BLUE_IN_RGB);

        xil_printf("||| %s |||\n", (char *)pvParameters);
        vTaskDelay(xDelay1500ms);
    }
}

static void Yellow_Task(void *pvParameters) {

    const TickType_t xDelay2500ms = pdMS_TO_TICKS(2500UL);

    for (;;) {

        // Write the bit value (color code) to the white leds
        XGpio_DiscreteWrite(&LEDInst, 1, 0b0010);

        // Write the bit value (color code) to the RGB LED
        /***CHANGE THE RGB LED COLOR HERE***/
        XGpio_DiscreteWrite(&RGBInst, 1, YELLOW_IN_RGB);

        xil_printf("|| %s ||\n", (char *)pvParameters);
        vTaskDelay(xDelay2500ms);
    }
}
