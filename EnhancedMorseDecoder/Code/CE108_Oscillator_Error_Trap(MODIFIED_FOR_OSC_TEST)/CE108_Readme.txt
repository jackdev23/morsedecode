
                              Readme File for Code Example:
             CE108 - Oscillator failure traps and Fail-safe Clock Monitoring
             ---------------------------------------------------------------

This file contains the following sections:
1. Code Example Description
2. Folder Contents
3. Suggested Development Resources
4. Reconfiguring the project for a different dsPIC33F device
5. Revision History


1. Code Example Description:
----------------------------
Microchip's 16-bit dsPIC� Digital Signal Controllers feature an on-chip mechanism to detect oscillator/clock failures.
In addition, the processor features the ability to continue code execution using an internal Fast RC oscillator.

The attached code example demonstrates:
a. How the Fail-Safe Clock Monitor module may be enabled via config macros.
b. How to write an effective Oscillator Failure trap service routine.

In this example, the device is configured to operate off an external canned oscillator running via the PLL. 
The code in the main.c file simply toggles a general purpose I/O port pin,RA6.
The user may place an oscilloscope probe on RA6 to verify the frequency at which the it is being toggled.
The user may then simply disconnect the canned oscillator from the board or ground it's output to simulate a clock failure.
The device continues to operate using the Internal FRC oscillator. 
The CPU vectors to the Oscillator Failure trap and clears the trap condition via a special write sequenc 
to the OSCCONL register. 

Further, a software flag is set if the PLL was found to be out-of-lock.

Note: In this project clock switching and clock monitoring has been enabled. MPLAB IDE will warn the user 
to perform a POR on the board in this case. This is being done because when clock switching has been enabled, 
the device uses the clock active prior to the last MCLR reset event (specified in the COSC bits in OSCCON register) 
rather than the clock settings specified by the FOSC fuse. In order for the device to use the clock source specified 
by the FOSC fuse, a POR reset event must take place.

2. Folder Contents:
-------------------
This folder contains the following sub-folders:
a. C:\Program Files\Microchip\MPLAB C30\support\gld
        This folder will have the device GLD file, it is used for building the project. 
	This file was provided with the MPLAB� C30 toolsuite.

b. C:\Program Files\Microchip\MPLAB C30\support\h
        This folder contains C header files useful in building this
        project. Device register and bit definitions are provided in
        the *.h file that follows the device name. These files were provided
        with the MPLAB� C30 toolsuite.

c. C:\Program Files\Microchip\MPLAB C30\lib
        This folder contains library archive files, which are a
        collection of precompiled object files. The file
        named "libpic30-coff.a" contains the C run-time start-up
        library. These file were provided with the
        MPLAB� C30 toolsuite.

d. hex
        This folder contains three file types - coff, hex and map.
        These are files generated by the MPLAB� C30 toolsuite on build
        operation performed within MPLAB� IDE. The *.map file contains
        details on memory allocation for various variables, constants
        and dsPIC instructions specified in the source and library
        code. The *.hex file contains a binary file that may be
        programmed into the dsPIC device. The *.coff file contains
        a binary file that is used by MPLAB� IDE for simulation.

e. h
        This folder contains include files for the code example.


f. src
        This folder contains all the C and Assembler source files (*.c,
        *.s) used in demonstrating the described example. This folder
        also contains a sub-folder named "obj" that stores compiled
        object files generated when the project is built.


3. Suggested Development Resources:
-----------------------------------
        a. Explorer 16 Demo board with dsPIC33FJ256GP710 controller

4. Reconfiguring the project for a different dsPIC33F device:
-------------------------------------------------------------
The Project/Workspace can be easily reconfigured for any dsPIC33F device.
Please use the following general guidelines:
        a. Change device selection within MPLAB� IDE to a dsPIC33F device of
        your choice by using the following menu option:
        MPLAB IDE>>Configure>>Select Device

        b. Provide the correct device linker script and header file for your
        device. Device linker scripts and header files are available in your
        MPLAB� C30 installation folder under:
        Device Linker Script-
                YourDrive:>Program Files\Microchip\MPLAB C30\support\gld
        Device C Header file-
                YourDrive:>Program Files\Microchip\MPLAB C30\support\h
        Device ASM Include file-
                YourDrive:>Program Files\Microchip\MPLAB C30\support\inc

        c. Provide the appropriate path to your MPLAB C30 support file locations
        using the menu option:
        MPLAB IDE>>Project>>Build Options>>Project

        d. Chose the development board applicable to your device. Some options
        are provided below:


        e. Re-build the MPLAB� project using the menu option:
        MPLAB IDE>>Project>>Build All

        f. Download the hex file into the device and run.

5. Revision History :
---------------------
        04/01/2006 - Initial Release of the Code Example