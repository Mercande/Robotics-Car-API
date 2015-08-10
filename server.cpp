/**
 * This file is part of Robotics-Car-API, RestAPI on RaspberryPi B+.
 *
 * Copyright (c) 2015 Robotics-Car-API contributors (http://mercandalli.com)
 *
 * LICENSE:
 *
 * Robotics-Car-API is free software: you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any
 * later version.
 *
 * Robotics-Car-API is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * @author E.Mercandalli J.Mercandalli
 * @license http://www.gnu.org/licenses/gpl.html
 * @copyright 2015 Robotics-Car-API contributors (http://mercandalli.com)
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

// Jsons
#include "rapidjson/include/rapidjson/document.h"
#include <cstdio>

//Hardware Raspberry
#ifdef __APPLE__
#else
	#include "wiringPi/wiringPi.h"
	#include "wiringPi/wiringPiI2C.h"

	//Run python sudo apt-get install python-dev
	#include "/usr/include/python2.7/Python.h"
#endif


// Server
#define MAX_SIZE 900000
#define PORT 8888

// Hardware
extern const int ID_LED_1			= 1;
extern const int ID_DISTANCE_1	= 2;
extern const int ID_DISTANCE_2	= 3;


/************************************************************/
/**** Utils                                              ****/
/************************************************************/

void printLine() {
	printf("------------------------------------------------\n");
}

int convertBcmToWiring(int pin_bcm) {
	switch(pin_bcm) {
		case 18: return 1;
		case 23: return 4;
		case 24: return 5;
		case 25: return 6;
		case 4: return 7;
		case 17: return 0;
		case 22: return 3;
	}
	return 1;
}


/************************************************************/
/**** Hardware functions                                 ****/
/************************************************************/

int gpio_write_pin(int pin_bcm, int on_off) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
		wiringPiSetup() ;
	  	pinMode(pin, OUTPUT) ;
		digitalWrite(pin, on_off);
	#endif

	return 0;
}

double gpio_read_pin(int pin) {
	// TODO

	return 0;
}

double gpio_read_distance(int device) {
	// TODO

	return 0;
}


/************************************************************/
/**** Functions called by the server                     ****/
/************************************************************/

int gpio_write(int id, double value) {
	switch(id) {
		// LED 1
		case ID_LED_1 : {
			return gpio_write_pin(18, value);			
		}

		// DISTANCE 1
		case ID_DISTANCE_1: {
			return -1;
		}

		// DISTANCE 2
		case ID_DISTANCE_2: {
			return -1;
		}
	}
	return -1;
}

double gpio_read(int id) {
	switch(id) {

		// LED 1
		case ID_LED_1:		
			return gpio_read_pin(18);
		break;

		// DISTANCE 1
		case ID_DISTANCE_1:
			// TODO return gpio_read_distance(int device);
		break;

		// DISTANCE 2
		case ID_DISTANCE_2:
			// TODO return gpio_read_distance(int device);
		break;
	}
	return 0;
}



/************************************************************/
/**** JSON Parsing  ****/
/************************************************************/

int json_parse_body(char* body) {
	if(body == NULL) {
		return -1;
	}

	// TODO

	return 0;
}



/************************************************************/
/**** Test functions called each time the program start  ****/
/************************************************************/

int test_hardware() {

	printf("\n");
	printLine();
	printf("TEST : HARDWARE\n\n");	

	int time =  200;
	for(int i = 0; i< 25; i++) {
		gpio_write(1, 1);
		delay(time);
		gpio_write(1, 0);
		delay(time);
	}	

	printf("\n");
	printLine();

	return 0;
}

int test_json() {

	// JSON Tuto https://code.google.com/p/rapidjson/wiki/UserGuide
	const char json[] = "{ \"succeed\" : true, \"id\" : 1988, \"value\" : \"toto\", \"pi\" : 3.14, \"n\": null, \"a\": [1,2,3]}";

	printf("\n");
	printLine();
	printf("TEST : JSON with: %s\n\n", json);	

    rapidjson::Document document;
    document.Parse<0>(json);

    printf("value = %s\n", document["value"].GetString());

    assert(document["id"].IsNumber());       // Number is a JSON type, but C++ needs more specific type.
	assert(document["id"].IsInt());          // In this case, IsUint()/IsInt64()/IsUInt64() also return true.
    printf("id = %d\n", document["id"].GetInt());

    assert(document["pi"].IsNumber());
	assert(document["pi"].IsDouble());
	printf("pi = %g\n", document["pi"].GetDouble());

	assert(document["succeed"].IsBool());         // JSON true/false are bool. Can also uses more specific function IsTrue().
	printf("succeed = %s\n", document["succeed"].GetBool() ? "true" : "false");

	printf("n = %s\n", document["n"].IsNull() ? "null" : "?");

	const rapidjson::Value& a = document["a"]; // Using a reference for consecutive access is handy and faster.
	assert(a.IsArray());
	for (rapidjson::SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
    	printf("a[%d] = %d\n", i, a[i].GetInt());

    printLine();

    return 0;
}

void test() {
	test_json();
	test_hardware();
}





/************************************************************/
/**** SERVER                                             ****/
/************************************************************/

int main()
{

	test();

	// Two socket descriptors which are just integer numbers used to access a socket
	int sock_descriptor, conn_desc;

	// Two socket address structures - One for the server itself and the other for client
	struct sockaddr_in serv_addr, client_addr;

	// Buffer to store data read from client
	char buff[MAX_SIZE];

	// Create socket of domain - Internet (IP) address, type - Stream based (TCP) and protocol unspecified
	// since it is only useful when underlying stack allows more than one protocol and we are choosing one.
	// 0 means choose the default protocol.
	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	// A valid descriptor is always a positive value
	if(sock_descriptor < 0)
		printf("Failed creating socket\n");

	// Initialize the server address struct to zero
	bzero((char *)&serv_addr, sizeof(serv_addr));

	// Fill server's address family
	serv_addr.sin_family = AF_INET;

	// Server should allow connections from any ip address
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	// 16 bit port number on which server listens
	// The function htons (host to network short) ensures that an integer is interpretted
	// correctly (whether little endian or big endian) even if client and server have different architectures
	serv_addr.sin_port = htons(PORT);

	// Attach the server socket to a port. This is required only for server since we enforce
	// that it does not select a port randomly on it's own, rather it uses the port specified
	// in serv_addr struct.
	if (bind(sock_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Failed to bind\n");
		close(sock_descriptor);
		return 0;
	}

	while(1) {

		// Server should start listening - This enables the program to halt on accept call (coming next)
		// and wait until a client connects. Also it specifies the size of pending connection requests queue
		// i.e. in this case it is 5 which means 5 clients connection requests will be held pending while
		// the server is already processing another connection request.
		listen(sock_descriptor, 5);

		printf("\nWaiting for connection on %d...\n\n", PORT);
		socklen_t size = sizeof(client_addr);

		// Server blocks on this call until a client tries to establish connection.
		// When a connection is established, it returns a 'connected socket descriptor' different
		// from the one created earlier.
		conn_desc = accept(sock_descriptor, (struct sockaddr *)&client_addr, &size);         
		if (conn_desc == -1)
			printf("Failed accepting connection\n");
		else
			printf("Connected\n");


		uint16_t networkLen;
	    read(conn_desc, &networkLen, sizeof(networkLen));

	    uint16_t len = ntohs(networkLen); // convert back to host byte order
	    if ( read(conn_desc, buff, sizeof(buff) - 1)> 0) {

	    	buff[len] = '\0';
	    	printf("Received %u : %s\n", len, buff);

			// The new descriptor can be simply read from / written up just like a normal file descriptor
			if ( write(conn_desc, buff, len-1) > 0)
				printf("Written %s", buff);
			else
				printf("Failed writing\n");
		}
		else
			printf("Failed receiving\n");


	    

	    

		/*

		// The new descriptor can be simply read from / written up just like a normal file descriptor
		if ( read(conn_desc, buff, sizeof(buff)-1) > 0) {
			printf("Received %s", buff);

			// The new descriptor can be simply read from / written up just like a normal file descriptor
			if ( write(conn_desc, buff, sizeof(buff)-1) > 0)
				printf("Written %s", buff);
			else
				printf("Failed writing\n");
		}
		else
			printf("Failed receiving\n");

		*/

		printf("\n");

		// Program should always close all sockets (the connected one as well as the listening one)
		// as soon as it is done processing with it
		close(conn_desc);
	}

	close(sock_descriptor);
 
	return 0;
}
