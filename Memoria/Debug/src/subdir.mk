################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Memoria.c \
../src/cliente_conexion.c \
../src/conexiones.c \
../src/conexionesMatelib.c \
../src/configMemoria.c \
../src/logs.c \
../src/mem.c \
../src/signals.c \
../src/tests.c \
../src/tlb.c 

OBJS += \
./src/Memoria.o \
./src/cliente_conexion.o \
./src/conexiones.o \
./src/conexionesMatelib.o \
./src/configMemoria.o \
./src/logs.o \
./src/mem.o \
./src/signals.o \
./src/tests.o \
./src/tlb.o 

C_DEPS += \
./src/Memoria.d \
./src/cliente_conexion.d \
./src/conexiones.d \
./src/conexionesMatelib.d \
./src/configMemoria.d \
./src/logs.d \
./src/mem.d \
./src/signals.d \
./src/tests.d \
./src/tlb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2021-2c-En-Casa-Compilaba/matelib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


