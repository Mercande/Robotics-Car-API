OS:=$(shell uname)
OPT:=

ifeq ($(OS),Darwin)
	OPT:=
else
	OPT:="-lwiringPi"
endif

all:
	@echo "Compile for OS $(OS)..."
	g++ -o server server.cpp $(OPT)
	