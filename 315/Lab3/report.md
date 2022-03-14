---
title: ECE 315 Lab 3 Reportussy
subtitle: Winter 2022
fontsize: 12pt
geometry: margin=0.75in
author: Charles Ancheta (1581672), Pushkar Sabharwal (1588927)
---

# Objectives

# Exercise 1

## Design Summary

<!-- TODO: Write brief summary of design -->

# Exercise 2

## Design Summary

<!-- TODO: Write brief summary of design -->

# Discussion

1. Justify the need for each of the critical sections that you identified in the driver functions. What could happen if a critical section is not protected?

2. Why should user tasks not be required to enable and disable interrupts in either the receive or transmit directions?

Enabling and disabling interrupts are hardware-specific and should be abstracted away for these user tasks using drivers or libraries.

3. In the interrupt service routine, does it matter if the transmit interrupts are handled before the receive interrupts, or vice versa? Is there is a better order for handling interrupts in the receive and transmit directions? Explain why one order is better.

4. Why must transmit interrupts be disabled when there is no more data to transmit? What would happen if transmit interrupts were to be left enabled in that case?

5. Can receive interrupts be left on, or should they ever be disabled?

6. Justify the numbers that you found being produced by the three status messages for the blocks of characters that were send to the Zybo Z7 board. If possible, use a small example to explain these numbers.
