---
title: ECE 315 Lab 3 Reportussy
subtitle: Winter 2022
fontsize: 12pt
geometry: margin=0.75in
author: Charles Ancheta (1581672), Pushkar Sabharwal (1588927)
---

# Objectives

The objective of this lab is to explore SPI communication with both the master and slave modes. We also aim to use `vTaskGetRunTimeStats()` to measure the load of an artificially expensive computing task.

# Exercise 1

In this exercise, our goal is to send and receive bytes through the SPI interface and count the number of bytes that went through.

## Design Summary

The program contains three tasks, `TaskUartManager`, `TaskSpi0Master`, and `TaskSpi1Slave`. There are two flags that we can toggle to enable loopback between the UART and the SPI master. If UART loopback is enabled, the program simply prints back the message without moving bytes through any queues. If the SPI master loopback is enabled, it moves the bytes through the FIFO queues without sending them through the SPI. The following diagram illustrates this more clearly:

![](diagram.jpg)

When the terminating sequence is sent, the UART task sends dummy control characters to make the Master "push out" the message from the Slave task through the SPI.

# Exercise 2

In this exercise, our goal is to find the `loop_count` necessary to affect the CPU load.
