OS:=$(shell uname)
HARDWARE_OPTION:=
CC:=g++

ifeq ($(OS),Darwin)
	HARDWARE_OPTION:=
else
	HARDWARE_OPTION:=-lwiringPi
endif

all: server

server: server.o hardware.o
	@echo "Compile for OS $(OS)..."
	$(CC) -o server server.o hardware.o

server.o: server.cpp hardware.h
	$(CC) -c server.cpp

hardware.o: hardware.cpp
	$(CC) -c hardware.cpp	$(HARDWARE_OPTION)

clean:
	rm *o server.o
	rm *o hardware.o