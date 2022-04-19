################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Swamp.c \
../src/conexiones.c \
../src/configSwamp.c \
../src/logger.c \
../src/manejo.c 

OBJS += \
./src/Swamp.o \
./src/conexiones.o \
./src/configSwamp.o \
./src/logger.o \
./src/manejo.o 

C_DEPS += \
./src/Swamp.d \
./src/conexiones.d \
./src/configSwamp.d \
./src/logger.d \
./src/manejo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


