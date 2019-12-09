# PES Project 6 Readme
Jack Campbell

- [PES Project 6 Readme](#pes-project-6-readme)
  - [Description](#description)
  - [Observations](#observations)
  - [Installation/Execution Notes](#installationexecution-notes)
    - [Running the FB builds](#running-the-fb-builds)
  - [CODE](#code)

## Description
This repo contains custom sources and makefiles for Project 6 as well as adapted and generated code 
from MCUXpresso and the KL25Z SDK.

This project contains two configurations: Debug and Status. The debug build prints out all of the individual sample reads/writes.

I removed the log_data and log_integer functionality from my logger, since I felt it wasn't needed with the 
LOG_STRING_ARGS macro I have and they were wasting SRAM with their function-static character buffers.

## Observations

This was a good exercise to get used to creating a real-time program as a series of tasks. There were hiccups
when getting used to what was and wasn't allowed (any function using FreeRTOS calls needed to be a task, and all tasks
are not allowed to return).

I had trouble getting the DMA interrupt to fire, so I just used a polling/callback approach to finish the other parts of
the assignment.

It took me awhile to learn that I needed to physically connect the DAC and ADC pins. Once I did that, my code all worked.

## Installation/Execution Notes

These are the steps to build the project in MCUXpresso.

1) Clone the repo
2) In MCUXpresso, click `New > Project`.
3) Select `Makefile project with existing code...`
4) Unselect C++, enter a project name, browse to the directory of the repo, and select `NXP MCU Tools`, then hit next.
5) Now set up the configurations. Right click the project,
6) Hit Properties
7) Uncheck "Generate makefiles"
8) Add "Debug" to the build directory path in the same dialog.
9) Do the same for Normal and Test configurations.

### Running the FB builds

1) Right click on the project name in the file hierarchy, select `Debug as > Debug configurations...`
2) Select `GDB PEMicro Interface Debugging`
3) Hit `New launch configuration`
4) Select a name for the output configuration (you need one for both Release and Debug)
5) Set the `C/C++ Application` field to the binary you want to run, either `Debug/output/kl25z_debug.axf` for Debug or `Release/output/kl25z_run.axf` for Release
6) Hit Apply
7) Hit Debug
8) The program should run in the console below, provided the board is connected successfully.

## CODE


