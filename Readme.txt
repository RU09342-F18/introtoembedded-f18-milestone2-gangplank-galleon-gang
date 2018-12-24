Milestone 2 - Closed Loop Cooling System
Authors: Joey McNatt and Ian Moffitt
Board: MSP430F5529
Program: main.c

This program is a simple closed loop cooling system. It accepts a desired temperature in degrees Celsius via UART from the USB Port, 
and uses that along with current temperature information taken from a PTAT to determine how much to use a fan to cool the system.
This solves the issue of cooling without leaving a fan running the whole time. More detailed

To use, flash main.c onto an MSP430F5529 and connect all pins to the correct circuit elements, tie grounds together, and connect the
micro USB on the board to a computer with RealTerm (or similar) installed. Once the system boots up, open its port, and send the desired
temperature. The board will echo this information back and send temperature updates via UART constantly.
Fan MOSFET: 2.0
Temperature Sensor: 6.0
