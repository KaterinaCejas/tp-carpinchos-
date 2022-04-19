################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/conexionesMateLib.c \
../src/deadlock.c \
../src/mateLibKernel.c \
../src/planificacion.c 

OBJS += \
./src/Kernel.o \
./src/conexionesMateLib.o \
./src/deadlock.o \
./src/mateLibKernel.o \
./src/planificacion.o 

C_DEPS += \
./src/Kernel.d \
./src/conexionesMateLib.d \
./src/deadlock.d \
./src/mateLibKernel.d \
./src/planificacion.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2021-2c-En-Casa-Compilaba/matelib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


