################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../bezier.cpp \
../core.cpp \
../coreglx.cpp \
../grid.cpp \
../joystick.cpp \
../keyboard.cpp \
../main.cpp \
../mouse.cpp \
../path.cpp \
../point.cpp \
../quadrix.cpp \
../quat.cpp \
../rend.cpp \
../timer.cpp \
../ttf.cpp \
../vector.cpp 

OBJS += \
./bezier.o \
./core.o \
./coreglx.o \
./grid.o \
./joystick.o \
./keyboard.o \
./main.o \
./mouse.o \
./path.o \
./point.o \
./quadrix.o \
./quat.o \
./rend.o \
./timer.o \
./ttf.o \
./vector.o 

CPP_DEPS += \
./bezier.d \
./core.d \
./coreglx.d \
./grid.d \
./joystick.d \
./keyboard.d \
./main.d \
./mouse.d \
./path.d \
./point.d \
./quadrix.d \
./quat.d \
./rend.d \
./timer.d \
./ttf.d \
./vector.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


