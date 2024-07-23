################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../incl/Blossom/PMduals.cpp \
../incl/Blossom/PMexpand.cpp \
../incl/Blossom/PMinit.cpp \
../incl/Blossom/PMinterface.cpp \
../incl/Blossom/PMmain.cpp \
../incl/Blossom/PMrepair.cpp \
../incl/Blossom/PMshrink.cpp \
../incl/Blossom/misc.cpp 

OBJS += \
./incl/Blossom/PMduals.o \
./incl/Blossom/PMexpand.o \
./incl/Blossom/PMinit.o \
./incl/Blossom/PMinterface.o \
./incl/Blossom/PMmain.o \
./incl/Blossom/PMrepair.o \
./incl/Blossom/PMshrink.o \
./incl/Blossom/misc.o 

CPP_DEPS += \
./incl/Blossom/PMduals.d \
./incl/Blossom/PMexpand.d \
./incl/Blossom/PMinit.d \
./incl/Blossom/PMinterface.d \
./incl/Blossom/PMmain.d \
./incl/Blossom/PMrepair.d \
./incl/Blossom/PMshrink.d \
./incl/Blossom/misc.d 


# Each subdirectory must supply rules for building sources it contributes
incl/Blossom/%.o: ../incl/Blossom/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


