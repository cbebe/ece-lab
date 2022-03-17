---
title: ECE 315 Lab 4 Report
subtitle: Winter 2022
fontsize: 12pt
geometry: margin=0.75in
author: Charles Ancheta (1581672), Pushkar Sabharwal (1588927)
---

# Objectives

The objective of this lab is to gain experience in operating a stepper motor in FreeRTOS. We also would try safety-critical design by adding an emergency stop button to the motor.

## Design Summary

The program contains three tasks. `_Task_Uart` is responsible for the user interface, parsing input into the stepper motor parameters. `_Task_Motor` is responsible for moving the stepper motor. `_Task_Emerg_Stop` is responsible for polling the emergency stop button and immediately stopping the stepper motor.

# Exercise 1

In this exercise, our goal is to move the stepper motor with the `_Task_Motor` task using a sequence of steps that are parsed from user input.

First, we wait for motor parameters to arrive in the FIFO with the UART task. We then set the stepper motor parameters to match the received parameters. After that, we start moving the motor using relative steps with the given sequences. We then reset the sequence and parameter flags to be ready for user input again.

# Exercise 2

In this exercise, our goal is to implement an emergency stop button in the `_Task_Emerg_Stop` task to make the stepper motor immediately decelerate in case of emergency.

We first read the input from the button and the increment the button state if it's pressed. If the button is pressed for 3 poll operations in a row, we will then activate the emergency stop. This suspends the two other tasks, disables the motor, and do an infinite loop on a flashing red light. To start over, the user will have to reset the board and possibly flash the program again.
