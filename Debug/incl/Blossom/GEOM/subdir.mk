################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../incl/Blossom/GEOM/GPMinit.cpp \
../incl/Blossom/GEOM/GPMinterface.cpp \
../incl/Blossom/GEOM/GPMkdtree.cpp \
../incl/Blossom/GEOM/GPMmain.cpp 

OBJS += \
./incl/Blossom/GEOM/GPMinit.o \
./incl/Blossom/GEOM/GPMinterface.o \
./incl/Blossom/GEOM/GPMkdtree.o \
./incl/Blossom/GEOM/GPMmain.o 

CPP_DEPS += \
./incl/Blossom/GEOM/GPMinit.d \
./incl/Blossom/GEOM/GPMinterface.d \
./incl/Blossom/GEOM/GPMkdtree.d \
./incl/Blossom/GEOM/GPMmain.d 


# Each subdirectory must supply rules for building sources it contributes
incl/Blossom/GEOM/%.o: ../incl/Blossom/GEOM/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


