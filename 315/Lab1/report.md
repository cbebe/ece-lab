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

First, we define the global variables to complete the execution of this part, for this part we define the delay that prevents the SSD from flickering to be 15. Then we define the right part of the seven-segment display as 0, and left part of the seven-segment display as 1, we also define output direction mask as 0x00. Then we just need to modify the SSD_Decode() function by adding more switch cases. We check for the value 'key_value' from '0'-'F' in hexadecimal value and for each value, and define an 8 bit binary value for each one. For instance, for the case '0', we first check if the value of the 'cathode' value is 0, if it's 0, then we return 0b00111111 otherwise, we define it as 0b10111111.

# Exercise 2: Entering multiple digits using the keypad to perform logical and modulo operations

In part 2 of the lab, we are required to implement a system in which, when a keypad key corresponding to a decimal digit is pressed, the corresponding digit will be displayed on the sdk terminal. Inputs from the keys A, B, C, D and F are to be ignored. The program is then required to interpret the two inputted digits as operands and give the ability to perform various functions like AND, OR, XOR and Modulo function using the operands.

# Exercise 3: A Simple calculator

In part 3 of the lab, our goal is to program a simple calculator which can perform four-functions. These operations include addition, subtraction, multiplication and the ability to check if the entered number is a palindrome which is selected using the keys ‘A’, ‘B’, ‘C’ and ‘D’ respectively. The answer of all the operations will be shown in the SDK terminal.

We first implement 'prvRxTask()' function to read three elements from the queue using 'xQueueReceive()' function which are the two operands and the operator. Then we have switch case to check for the type of the operator. Then call the function 'doBinaryOperation' and pass the operands and the type of overflow to the function. In the case of checking the palindrome, we call the function 'checkPalindromes' and pass the operands to it. The 'doBinaryOperation' function is a while loop that checks for the overflow first. If no overflow exists then we print the result using the 'printResult'.