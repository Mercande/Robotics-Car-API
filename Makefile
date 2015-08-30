OS:=$(shell uname)
HARDWARE_COMPILATION:=
CC:=g++

ifeq ($(OS),Darwin)
	HARDWARE_COMPILATION:=-std=c++0x -pthread
else
	HARDWARE_COMPILATION:=pca/pca9685.c -lwiringPi -std=c++0x -pthread
endif

all:
	@echo "Compile for OS $(OS)..."
	$(CC) -o server server.cpp hardware.cpp utils.cpp $(HARDWARE_COMPILATION)
	rm -f *o server.o
