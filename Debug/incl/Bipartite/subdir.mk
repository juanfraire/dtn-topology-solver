################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../incl/Bipartite/matrix.cpp \
../incl/Bipartite/munkres.cpp 

OBJS += \
./incl/Bipartite/matrix.o \
./incl/Bipartite/munkres.o 

CPP_DEPS += \
./incl/Bipartite/matrix.d \
./incl/Bipartite/munkres.d 


# Each subdirectory must supply rules for building sources it contributes
incl/Bipartite/%.o: ../incl/Bipartite/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


