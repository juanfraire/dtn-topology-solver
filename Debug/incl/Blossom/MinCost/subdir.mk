################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../incl/Blossom/MinCost/MinCost.cpp 

OBJS += \
./incl/Blossom/MinCost/MinCost.o 

CPP_DEPS += \
./incl/Blossom/MinCost/MinCost.d 


# Each subdirectory must supply rules for building sources it contributes
incl/Blossom/MinCost/%.o: ../incl/Blossom/MinCost/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


