################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Mylib_GPIO.c \
../User/Mylib_Sleep.c \
../User/Mylib_Timer.c \
../User/System.c \
../User/Timer_2024_Kit.c \
../User/ch32v00x_it.c \
../User/main.c \
../User/system_ch32v00x.c 

OBJS += \
./User/Mylib_GPIO.o \
./User/Mylib_Sleep.o \
./User/Mylib_Timer.o \
./User/System.o \
./User/Timer_2024_Kit.o \
./User/ch32v00x_it.o \
./User/main.o \
./User/system_ch32v00x.o 

C_DEPS += \
./User/Mylib_GPIO.d \
./User/Mylib_Sleep.d \
./User/Mylib_Timer.d \
./User/System.d \
./User/Timer_2024_Kit.d \
./User/ch32v00x_it.d \
./User/main.d \
./User/system_ch32v00x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\user\share\NPC\MounRiver(WCH)\CH32V003F4P6\CH32V003F4P6_Timer_2024_Kit\CH32V003F4P6_Timer_2024_Kit\Debug" -I"C:\Users\user\share\NPC\MounRiver(WCH)\CH32V003F4P6\CH32V003F4P6_Timer_2024_Kit\CH32V003F4P6_Timer_2024_Kit\Core" -I"C:\Users\user\share\NPC\MounRiver(WCH)\CH32V003F4P6\CH32V003F4P6_Timer_2024_Kit\CH32V003F4P6_Timer_2024_Kit\User" -I"C:\Users\user\share\NPC\MounRiver(WCH)\CH32V003F4P6\CH32V003F4P6_Timer_2024_Kit\CH32V003F4P6_Timer_2024_Kit\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

