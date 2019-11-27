################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/circular_buffer.c \
../source/dac.c \
../source/handle_led.c \
../source/logger.c \
../source/main.c \
../source/mtb.c \
../source/post.c \
../source/semihost_hardfault.c \
../source/setup_teardown.c \
../source/sine.c \
../source/time.c 

OBJS += \
./source/circular_buffer.o \
./source/dac.o \
./source/handle_led.o \
./source/logger.o \
./source/main.o \
./source/mtb.o \
./source/post.o \
./source/semihost_hardfault.o \
./source/setup_teardown.o \
./source/sine.o \
./source/time.o 

C_DEPS += \
./source/circular_buffer.d \
./source/dac.d \
./source/handle_led.d \
./source/logger.d \
./source/main.d \
./source/mtb.d \
./source/post.d \
./source/semihost_hardfault.d \
./source/setup_teardown.d \
./source/sine.d \
./source/time.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DDEBUG -DCPU_MKL25Z128VLK4 -DFSL_RTOS_FREE_RTOS -DFRDM_KL25Z -DFREEDOM -DCPU_MKL25Z128VLK4_cm0plus -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -I../include -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../freertos -I../startup -O0 -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


