/*
 *	Lab 2: Part 2: UART in Interrupt Mode
 *
 *	ECE 315	2022	: Computer Interfacing
 *  Created on		: July 27, 2021
 *      Author		: Shyama M. Gandhi, Mazen Elbaz
 *
 *     ------------------------------------------------------------------------------------------------------------------------------
 *
 *     This is the main source file that uses the Xilinx's UART driver functions from the "uart_driver.h" header file.
 *     The final objective is to implement as interrupt mode receive side and interrupt mode transmit side of the UART.
 *
 *     Inside the interrupt service routine in the uart_Driver.h file, the received bytes will be written at the back of receive queue. (xQueue_for_receive)
 *     The bytes to be transmitted later on to UART console, will be read from the front of the front of the transmit queue. (xQueue_for_transmit)
 *	   Please remember that the queues are private data structures hidden from the user's main source file.
 *
 *     Overview of this main source .c file:
 *     The driver file does all the background work and the queue implementation, which is hidden from the user. It is the four functions MyIsReceiveData(), MyReceiveByte(), MyIsTransmitFull() and MySendByte(),
 *     that will be called from this main source code file.
 *     This file will also have the logic responsible for detecting the "\r#\r" and "\r%\r" termination sequence for displaying the interrupt statistics and resetting the variables respectively.
 *     It is in this file where students will enter the logic to change the letter capitalization.
 *     Please enter your code at the commented sections in this file.
 *
 *     ------------------------------------------------------------------------------------------------------------------------------
 */


#include "stdio.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xtime_l.h"

//this is the uart driver file where students will add the implementation as mentioned in the lab manual
#include "uart_driver.h"
#include "initialization.h"

#define CHAR_ESC				0x23	/* '#' character is used as termination sequence */
#define CHAR_CARRIAGE_RETURN	0x0D	/* '\r' character is used in the termination sequence */

void printString(char countMessage[]);

TaskHandle_t task_receiveuarthandle = NULL;
TaskHandle_t task_transmituarthandle = NULL;
BaseType_t resetFlag;
BaseType_t endingSeq;
int Countbytes; 								// to count the number of bytes received over the UART
extern QueueHandle_t xQueue_for_transmit;
extern QueueHandle_t xQueue_for_receive;
extern int CountRxIrq;							//variable to count the receive side interrupts
extern int CountTxIrq;							//variable to count the transmit side interrupts



//Function declaration for UART interrupt setup
extern int SetupInterruptSystem(INTC *IntcInstancePtr, XUartPs *UartInstancePtr, u16 UartIntrId);
//Initialization function for UART
extern int Initialize_UART();

//the four driver functions whose definitions are in "uart_driver.h" file.
extern BaseType_t MyIsReceiveData( void );
extern u8 MyReceiveByte( void );
extern BaseType_t MyIsTransmitFull( void );
extern void MySendByte( u8 Data );


int main()
{
	int Status;

	xTaskCreate(Task_UART_buffer_receive, "uart_receive_task", 1024, (void*)0, tskIDLE_PRIORITY, &task_receiveuarthandle);
	xTaskCreate(Task_UART_buffer_send, "uart_transmit_task", 1024, (void*)0, tskIDLE_PRIORITY, &task_transmituarthandle);

	xQueue_for_transmit = xQueueCreate( SIZE_OF_QUEUE, sizeof( u8 ));
	xQueue_for_receive  = xQueueCreate( SIZE_OF_QUEUE, sizeof( u8 ));
	CountRxIrq = 0;
	CountTxIrq = 0;
	Countbytes = 0;
	resetFlag = pdFALSE;
	endingSeq = pdFALSE;

	Status = Initialize_UART();
	if (Status != XST_SUCCESS){
		xil_printf("UART Initialization failed\n");
	}

	vTaskStartScheduler();

	while(1);

	return 0;

}

void Task_UART_buffer_receive(void *p){

	int Status;

	Status = SetupInterruptSystem(&InterruptController, &UART, UART_INT_IRQ_ID);
	if (Status != XST_SUCCESS){
		xil_printf("UART PS interrupt failed\n");
	}

	u8 returnFlag = 0;
	u8 restartFlag = 0;

	printString("Please enter the text and then press ENTER\n");
	printString("To display the status messages, enter the end-of-block sequence, \\r#\\r and then press ENTER\n");
	printString("To reset the ISR transmit and receive global counters as well as byte counter, enter the sequence \\r%\\r and then press ENTER\n");
	
	for( ;; ){
		while(1){
				u8 pcString;
				char write_to_queue_value;

				/*******************************************************/
				//add three lines of code here to check MyIsReceiveData() status, get the data byte using the MyReceiveByte(),
				//check the status of MyIsTransmitFull() function.
				//the return value of the MyReceiveByte() function must be stored inside the "pcString" variable.

				/*******************************************************/

				write_to_queue_value = (char) pcString;	//casted to "char" type.

				/*******************************************************/
				//write the code to change the capitalization from lower to upper case and vice versa for the byte present inside the
				//"write_to_queue_value" variable.
				//store the changed byte to "write_to_queue_value" itself.

			
				/*******************************************************/

				/*******************************************************/
				//write one line of code to increment the variable used as a byte counter for UART characters
				
				/*******************************************************/

				MySendByte(write_to_queue_value);

				//detect \r#\r
				if (returnFlag == 2 && write_to_queue_value == CHAR_CARRIAGE_RETURN){
					returnFlag = 0;
					taskYIELD(); //force context switch
				}else if (returnFlag == 1 && write_to_queue_value == CHAR_ESC){
					returnFlag = 2;
				}else if (write_to_queue_value == CHAR_CARRIAGE_RETURN){
					returnFlag = 1;
				}else{
					returnFlag = 0;
				}

				/*******************************************************/
				//detect \r%\r sequence here.
				//Once you have detected this sequence, reset the byte counter, transmit interrupt counter and receive interrupt counter to zero.
				//Also, print this message on the console: "Byte Counter, CountRxIrq && CountTxIrq set to zero\n\n"
				//the counter variables have been already given to you.
				//note that the received character byte is stored inside "write_to_queue_value" variable.

				
				/*******************************************************/
			}
	}
}


//print the provided number using driver functions
void printNumber(char number[]){
	for (int i = 0; i < 10; i++){
		if (number[i] >= '0' && number[i] <= '9'){
			while(MyIsTransmitFull() == pdTRUE);
			MySendByte(number[i]);
		}
	}
}

//print the provided string using driver functions
void printString(char countMessage[]){
	for (int i = 0; countMessage[i] != '\0'; i++){
		while(MyIsTransmitFull() == pdTRUE);
		MySendByte(countMessage[i]);
	}
}

void Task_UART_buffer_send(void *p){

	UBaseType_t uxPriority;

	for( ;; ) {
		uxPriority = uxTaskPriorityGet( NULL );
		while(1){

			//print the special messages for ending sequence
			char countArray[10];
			char CountRxIrqArray[10];
			char CountTxIrqArray[10];
			sprintf(countArray,"%d", Countbytes);
			sprintf(CountRxIrqArray,"%d", CountRxIrq);
			sprintf(CountTxIrqArray,"%d", CountTxIrq);
			printString("Number of bytes processed: ");
			printNumber(countArray);
			MySendByte(CHAR_CARRIAGE_RETURN);
			printString("Number of Rx interrupts: ");
			printNumber(CountRxIrqArray);
			MySendByte(CHAR_CARRIAGE_RETURN);
			printString("Number of Tx interrupts: ");
			printNumber(CountTxIrqArray);
			MySendByte(CHAR_CARRIAGE_RETURN);


			//perform context switch
			taskYIELD();
		}
	}
}

