OS:=$(shell uname)
HARDWARE_OPTION:=
CC:=g++

ifeq ($(OS),Darwin)
	HARDWARE_OPTION:=
else
	HARDWARE_OPTION:=-lwiringPi
endif

all:
	@echo "Compile for OS $(OS)..."
	$(CC) -o server server.cpp hardware.cpp utils.cpp $(HARDWARE_OPTION)
	rm -f *o server.o
