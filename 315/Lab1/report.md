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

# Exercise 2: Entering multiple digits using the keypad to perform logical and modulo operations

In part 2 of the lab, we are required to implement a system in which, when a keypad key corresponding to a decimal digit is pressed, the corresponding digit will be displayed on the sdk terminal. Inputs from the keys A, B, C, D and F are to be ignored. The program is then required to interpret the two inputted digits as operands and give the ability to perform various functions like AND, OR, XOR and Modulo function using the operands.

# Exercise 3: A Simple calculator

In part 3 of the lab, our goal is to program a simple calculator which can perform four-functions. These operations include addition, subtraction, multiplication and the ability to check if the entered number is a palindrome which is selected using the keys ‘A’, ‘B’, ‘C’ and ‘D’ respectively. The answer of all the operations will be shown in the SDK terminal.
