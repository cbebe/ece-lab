/*
 * ECE - 315 	: WINTER 2022
 *
 * LAB 2		: Implementation of UART in polled mode - PART 1
 * Created by	: Shyama M. Gandhi, Mazen Elbaz
 *
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * PLEASE NOTE THAT YOU NEED TO FINISH THE TEMPLATE FIRST IN ORDER TO MAKE THE EXERCISE WORK.
 * The implementation objective:
 * Input from the user in the form of Morse code up to 500 character long. User types in the Morse code and this is received using the TaskMorseMsgReceiver().
 * Queue12 transmits the received morse code characters to TaskMorseMsgProcessor (). TaskMorseMsgProcessor () translates the morse code characters to original message.
 * This is done using the already defined morse code for every character in the morseTranslator.h
 * Please remember that all the alphabets and digits have been considered. However, only some of the common special character/punctuation have been used.
 * In case, if user enters an unrecognized character, * will be displayed in the decoded text message on the console.
 * TaskMorseMsgProcessor() sends the decoded message via Queue23 to the TaskDecodedMsgTransmitter() task.
 * TaskDecodedMsgTransmitter () will display the message on the console.
 *
 * After you have entered the morse code, press ENTER, type # and again press enter. The decoded message will be displayed on the terminal.
 * In case you exceed the 500 limit, an error message should be displayed on the console. After that type the termination sequence and then continue entering your morse code again.
 *
 * Some test cases that students may use:
 *
 * 				...|-|.|...-|.|-.| .--|.-|...| -...|---|.-.|-.| ---|-.| .-|.--.|.-.|..|.-..| .----|--...|--..--| .----|----.|----.|---..|.-.-.-|
 * 				STEVEN WAS BORN ON APRIL 17, 1998.
 *
 * 				..|-|.----.|...| -.|..|-.-.|.| -|---| --|.|.|-| -.--|---|..-|
 * 				IT'S NICE TO MEET YOU
 *
 * 				....|---|.--| .-|.-.|.| -.--|---|..-| -..|---|..|-.|--.|..--..|
 * 				HOW ARE YOU DOING?
 *
 * -------------------------------------------
 * Explanation for what happens during the overflow i.e., when you exceed the 500 limit:
 *
 * The morse code for HI ROBERT is  ....|..| .-.|---|-...|.|.-.|-| and then press ENTER. This accounts for a total of 31 characters. (including the ENTER)
 * To cause overflow, you need to repeat entering this morse code sequence 16 times (496 characters) + 1 more time to cause the overflow. Hence, total 17 times.
 * This means 31 * (17-1) = 496 characters + four characters from the 17th time entered sequence will be lost as a result of overflow and wouldn't be translated.
 * As a result, when you enter the termination sequence after the error is displayed, the remaining characters except the first four characters will be translated from the 17th time entered sequence.
 *
 * TRY IT!
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */

/***************************** Include Files *********************************/
#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xscugic.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stdio.h"
#include "string.h"

// header file containing the morse code for different characters
#include "morseTranslator.h"

/************************** Constant Definitions *****************************/

//UART definitions from xparameters.h
#define UART_DEVICE_ID              XPAR_XUARTPS_0_DEVICE_ID
#define UART_BASEADDR		   		XPAR_XUARTPS_0_BASEADDR

//definitions for the terminating sequence characters
#define CHAR_HASH				0x23	/* '#' character is used as termination sequence */
#define CHAR_CARRIAGE_RETURN	0x0D	/* '\r' character is used in the termination sequence */

#define TASK_PROCESS_BUFFER 	500
#define RECEIVER_POLL_PERIOD_MS 20 // 50 Hz --> 20 ms

//queue limit definitions
#define XQUEUE_12_CAPACITY 		20
#define XQUEUE_23_CAPACITY 		30

/***************** Macros (Inline Functions) Definitions *********************/

static void TaskMorseMsgReceiver ( void *pvParameters );
static TaskHandle_t xTask_msgreceive;
static void TaskMorseMsgProcessor ( void *pvParameters );
static TaskHandle_t xTask_msgprocess;
static void TaskDecodedMsgTransmitter ( void *pvParameters );
static TaskHandle_t xTask_msgtransmit;

static void UART_print_queueerror_msg(char * str);

static QueueHandle_t xQueue_12 = NULL;	//queue between TaskMorseMsgReceiver and TaskMorseMsgProcessor
static QueueHandle_t xQueue_23 = NULL;  //queue between TaskMorseMsgProcessor and TaskDecodedMsgTransmitter

void morseToTextConverter(char input_char);

/************************** Function Prototypes ******************************/

int Intialize_UART(u16 DeviceId);

/************************** Variable Definitions *****************************/

XUartPs Uart_PS;		/* Instance of the UART Device */
XUartPs_Config *Config;	/* The instance of the UART-PS Config */

//these variables are also used in the morseTranslator.h, so study them carefully!
char char_morse_sequence[10];
char output_text_sequence[500];
int output_length;
int char_seq_length;

int main(){

	int Status;

	xTaskCreate( 	TaskMorseMsgReceiver,
					( const char * ) "T1",
					configMINIMAL_STACK_SIZE*5,
					NULL,
					tskIDLE_PRIORITY,
					&xTask_msgreceive );

	xTaskCreate( 	TaskMorseMsgProcessor,
					( const char * ) "T2",
					configMINIMAL_STACK_SIZE*5,
					NULL,
					tskIDLE_PRIORITY,
					&xTask_msgprocess );

	xTaskCreate(	TaskDecodedMsgTransmitter,
					( const char * ) "T3",
					configMINIMAL_STACK_SIZE*5,
					NULL,
					tskIDLE_PRIORITY,
					&xTask_msgtransmit );


	Status = Intialize_UART(UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("UART Polled Mode Example Test Failed\r\n");
	}

	xQueue_12 = xQueueCreate( XQUEUE_12_CAPACITY ,sizeof( u8 ) );

	xQueue_23 = xQueueCreate( XQUEUE_23_CAPACITY ,sizeof( u8 ) );


	/* Check the queue was created. */
	configASSERT(xQueue_12);
	configASSERT(xQueue_23);

	xil_printf("Please type in your morse code below to be translated to text\n\n");
	xil_printf("Remember the following: \r	1) Add a '|' after every letter\r");
	xil_printf("	2) One space between each word\r");
	xil_printf("	3) One carriage return (ENTER) to write on a new line\r");
	xil_printf("	4) And when you are done, type in the termination sequence\n\n");

	vTaskStartScheduler();

	while(1);
	return 0;
}


int Intialize_UART(u16 DeviceId)
{
	int Status;
	/*
	 * Initialize the UART driver so that it's ready to use.
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Use NORMAL UART mode. */
	XUartPs_SetOperMode(&Uart_PS, XUARTPS_OPER_MODE_NORMAL);

	return XST_SUCCESS;
}

static void TaskMorseMsgReceiver( void *pvParameters ){
	u8 RecvChar;
	const TickType_t xPeriod = pdMS_TO_TICKS(RECEIVER_POLL_PERIOD_MS);
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for( ; ; ){

		/***********************************************/
		// enforce polling frequency here using the Delay function. The 20ms period has been already defined in this task for you!

		/***********************************************/

		// Receive characters over UART if there's room in the queue
		if (uxQueueMessagesWaiting(xQueue_12) < XQUEUE_12_CAPACITY) {

			/***********************************************/
			//write the function to receive the UART character into "RecvChar" variable. 
			//HINT : the function to be used here is indicated in the lab handout.
			//find that function and study it from this link: https://xilinx.github.io/embeddedsw.github.io/uartps/doc/html/api/globals.html

			/***********************************************/
			if (xQueueSendToBack(xQueue_12, &RecvChar, portMAX_DELAY) != pdPASS) {
				UART_print_queueerror_msg("xQueueSendToBack: Could not write character to xQueue_12\n");
			}
		}
	}
}


//Some HINTS for TaskMorseMsgProcessor() function :
//Use a while loop that blocks on the queue12 and increments the "no_of_characters_read" variable.
//In case you receive the termination sequence, you want to call the function morseToTextConverter() to perform the translation.
//You can set the "break_loop" to TRUE if your conversion is successful and the morse code is within the 500 character limit. In case, if the message exceeds 500 limit, this variable stays FALSE.
//Based on this variable, you can send the corresponding translated or error message to the queue23.
//the variable "output_length" will store the length of the translated message or the error message to be sent to the queue23.
//the variable "error flag" will be 1 when the error/overflow condition is encountered else remains zero. This variable can be used to decide what message needs to be sent to the queue23.

static void TaskMorseMsgProcessor( void *pvParameters ){
	static char read_from_queue12_value[TASK_PROCESS_BUFFER]; //static because we don't want this array to change when we switch between the task functions...
	int no_of_characters_read; //keeps track of the number of characters read
	BaseType_t break_loop;
	int error_flag;

	for( ; ; ){
		no_of_characters_read = 0;
		break_loop = FALSE;
		output_length = 0;
		error_flag = 0;
		memset(output_text_sequence, 0, TASK_PROCESS_BUFFER);

		//this while loop runs if and only if we are still inside the 500 character limit!
		while(no_of_characters_read<TASK_PROCESS_BUFFER){

			/***********************************************/
			//block on the queue23 here and keep track of the number of characters received.
			//Once you have termination sequence \r#\r , use the function morseToTextConverter() to converted the morse code into text message.
			//update the "break_loop" variable accordingly. The "break_loop" variable will break this while loop and check the if() condition as provided below.
			/***********************************************/

		}

		if (!break_loop){
			/***********************************************/
			// Overflow, store the error message "Maximum message length exceeded. Message ignored.\rType in the termination sequence to translate the 'excess' characters that went above the array limit and caused overflow.\r\r\n"
			// set the "error_flag" variable accordingly and store the length of this error message in the "output_length" variable.
			// if you have declared a temporary variable to store the error message, copy it into the "read_from_queue12_value" array.
			/***********************************************/
		}

		/***********************************************/
		//Now we will send either the converted characters or the error message to the Queue 23. Use the array variable "read_from_queue12_value" as this contains your data.
		//HINT: use looping and based on "error_flag" variable value, send either the error message or the translated message to queue23.
		//the number of times your loop iterates = X variable that was used to keep the track of the characters before. You need to figure out this X variable!
		/***********************************************/

		if(error_flag == 1){
			memset(read_from_queue12_value, 0, TASK_PROCESS_BUFFER);
		}
	}
}

static void TaskDecodedMsgTransmitter( void *pvParameters ){

	u8 write_to_console;
	const TickType_t xPeriod = pdMS_TO_TICKS(RECEIVER_POLL_PERIOD_MS);
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for( ; ; ){
		while (xQueueReceive(xQueue_23, &write_to_console, portMAX_DELAY) != pdTRUE);

		while(1){
			//if the transmitter is full, wait else send the data...
			if(XUartPs_IsTransmitFull(UART_BASEADDR)){
				//delay function to wait for 20ms here if the UART transmitter is full!
				vTaskDelayUntil(&xLastWakeTime, xPeriod);
			}
			else{
				/***********************************************/
				//Send the byte to the UART using the variable "write_to_console"
				//HINT : the function to be used here is indicated in the lab handout.
				//find that function and study it from this link: https://xilinx.github.io/embeddedsw.github.io/uartps/doc/html/api/globals.html

				/***********************************************/
				break;
			}
		}
	}
}

/*
 * Writes a null-terminated string to the UART device set by UART_DEVICE_ID.
 */
static void UART_print_queueerror_msg(char * str) {
	int i = 0;
	while (TRUE) {
		XUartPs_SendByte(UART_BASEADDR, str[i]);
		++i;

		// Do not print the null byte terminator
		if (str[i] == '\0')
			return;
	}
}

