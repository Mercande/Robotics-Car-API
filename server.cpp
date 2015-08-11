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
#include <string>
#include <time.h>
#include <math.h>

// Jsons
#include "rapidjson/include/rapidjson/document.h"
#include <cstdio>

//Hardware Raspberry
#ifdef __APPLE__
#else
	#include "wiringPi/wiringPi.h"
	#include "wiringPi/wiringPiI2C.h"
#endif


// Server
#define MAX_SIZE 900000
#define PORT 8888

// Hardware
extern const int ID_LED_1		= 1;
extern const int ID_DISTANCE_1	= 2;
extern const int ID_DISTANCE_2	= 3;
extern const int ID_SERVO_1		= 4;
extern const int ID_SERVO_2		= 5;

using std::string;


int var_gl_fd;


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
		case 4:  return 7;
		case 17: return 0;
		case 22: return 3;
	}
	return 1;
}

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}


/************************************************************/
/**** Hardware functions                                 ****/
/************************************************************/

int gpio_init_pin(int pin_bcm, int input_output) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
		wiringPiSetup();
	  	//pinMode(pin, OUTPUT);
		pinMode(pin, input_output);
	#endif

	return 0;
}

int gpio_write_pin(int pin_bcm, int on_off) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
		wiringPiSetup() ;
	  	//pinMode(pin, OUTPUT) ;
		digitalWrite(pin, on_off);
	#endif

	return 0;
}

double gpio_read_pin(int pin_bcm) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
		wiringPiSetup() ;
	  	//pinMode(pin, OUTPUT) ;
		return digitalRead(pin);
	#endif

	return -1;
}

double i2c_read_distance(int ad) {
	// TODO ERIC

	// Reading distance and brightness: SRF08
	// (Use i2cdetect -y 1 command to find device address : here 0x71 and 0x72)

	int distance = 0;

	#ifdef __APPLE__
	#else
		wiringPiSetup();
		int fd_dist = wiringPiI2CSetup(ad);		// ad = 0x71 and 0x72
		if(fd_dist == -1) {
			printf("Can't setup the I2C device (distance SRF08)\n");
		 	return -1;
	  	}
		//printf ("Setup I2C device (distance SRF08) OK - numero : %d \n", fd_dist);

		// Started the distance measure
		int j = wiringPiI2CWriteReg8(fd_dist, 0, 0x51);
		//printf ("Write j (0 attendu ; -1 si pb) : %d \n", j);

		// waiting for measure
		delay (200); // default 680

		// reading the brightness
		int k = wiringPiI2CReadReg8(fd_dist, 1);
		//printf ("Brightness level: %d \n", k);

		if(k<0)
			return -1;

		// reading the distance
		int range1 = wiringPiI2CReadReg8(fd_dist, 2);
		int range2 = wiringPiI2CReadReg8(fd_dist, 3);
		distance = (range1 << 8) + range2;
		//printf ("Distance: %d \n", distance);
	#endif
	
	return distance < 0 ? -2 : distance;
}

int i2c_write_servo(int ad, int id_pwm, double value) {
	// TODO ERIC
	return 0;
}

double gpio_read_distance(int device) {
	switch(device) {
		case 1:	return i2c_read_distance(0x71);
		case 2: return i2c_read_distance(0x72);
	}
	return 0;
}

int gpio_write_servo(int device, double value) {
	switch(device) {
		case 1:	return i2c_write_servo(0x40, 1, value);
		case 2: return i2c_write_servo(0x40, 16, value);
	}
	return 0;
}




/************************************************************/
/**** Functions called by the server                     ****/
/************************************************************/

void gpio_init() {

	#ifdef __APPLE__
	#else	
		// Init pin LED
		gpio_init_pin(18, OUTPUT);
	#endif
}

int gpio_write(int id, double value) {
	printf("gpio_write(id=%d, value=%lf)\n", id, value);

	switch(id) {

		case ID_LED_1 :
			return gpio_write_pin(18, value);

		case ID_DISTANCE_1:
			return -1;

		case ID_DISTANCE_2:
			return -1;

		case ID_SERVO_1:
			return gpio_write_servo(1, value);

		case ID_SERVO_2:
			return gpio_write_servo(2, value);
	}
	return -1;
}

double gpio_read(int id) {
	double result = 0;

	switch(id) {

		case ID_LED_1:		
			result = gpio_read_pin(18);
		break;

		case ID_DISTANCE_1:
			result = gpio_read_distance(1);
		break;

		case ID_DISTANCE_2:
			result = gpio_read_distance(2);
		break;

		case ID_SERVO_1:
			result = -1;
		break;

		case ID_SERVO_2:
			result = -1;
		break;
	}

	printf("gpio_read(id=%d)  =  %lf\n", id, result);

	return result;
}



/************************************************************/
/**** Request Parsing                                    ****/
/************************************************************/

char* json_parse_body(int len_body, const char* body) {

	/*
	EX
	{"succeed":true,"toast":"Test toast","debug":"on\/off led","content":{"user":{"id":1,"username":"Jonathan"},"date_request":"2015-08-11 09:53:55","hardware":[{"id":1,"read":false,"value":"1","type":"led","succeed":true}],"init_hardware":false}}
	*/

	// TODO Add missing keys and return json response

	if(body == NULL) {
		return NULL;
	}

	if(body[0] != '{') {
		return NULL;
	}

	rapidjson::Document json;
	
	if (json.Parse<0>(body).HasParseError()) {
		printf("Error parsing (1) : %s\n", body);
		return NULL;
	}

	if(json.HasMember("succeed")) {
		if(json["succeed"].IsBool()) {
			printf("succeed = %s\n", json["succeed"].GetBool() ? "true" : "false");
		}
    }

    if(json.HasMember("toast")) {
    	printf("toast = %s\n", json["toast"].GetString());
    }

    if(json.HasMember("debug")) {
    	printf("debug = %s\n", json["debug"].GetString());
    }   

    if(json.HasMember("content")) {

    	const rapidjson::Value& content = json["content"];

    	if(content.HasMember("date_request")) {
	    	printf("date_request = %s\n", content["date_request"].GetString());
	    }

	    if(content.HasMember("hardware")) {
	    	const rapidjson::Value& hardware = content["hardware"]; // Using a reference for consecutive access is handy and faster.
			if(hardware.IsArray()) {

				for (rapidjson::SizeType i = 0; i < hardware.Size(); i++) { // rapidjson uses SizeType instead of size_t.
			    	
					const rapidjson::Value& current_hardware = hardware[i];

					
					if(current_hardware.HasMember("id")) {
						int current_hardware_id = current_hardware["id"].GetInt();

						
						if(current_hardware.HasMember("read")) {
							if(current_hardware["read"].IsBool()) {

								
								if(current_hardware["read"].GetBool()) {

									gpio_read(current_hardware_id);

								}
								else {

									if(current_hardware.HasMember("value")) {

										string value = current_hardware["value"].GetString();
								    	gpio_write(current_hardware_id, atof(value.c_str()));
								    }									

								}
								
							}
					    }					    

					}
					

			    }

			}
			
	    }
	}

	return NULL;
}

int get_body_length(char* request) {
	if(request == NULL) {
		return -1;
	}

	string request_str(request);
	string content_length_str_1("Content-Length: ");
	string content_length_str_2("Content-length: ");

	if(request_str.find(content_length_str_1) != std::string::npos) {
		char * pch = strstr(request,"Content-Length: ");
		strtok(pch," ");
		return atoi(strtok (NULL, " "));
	}
	else if(request_str.find(content_length_str_2) != std::string::npos) {
		char * pch = strstr(request,"Content-length: ");
		strtok(pch," ");
		return atoi(strtok (NULL, " "));
	}

	return 0;
}

int get_body(const int len, const char* request, const int length_body, char* body) {

	int request_length = len;
	for(int i = 300 ; i < len ; i++ ) {
		request_length = i;
		if(request[i] == '\0')
			break;
	}
	for(int i = 1 ; i <= length_body ; i++ ) {
		body[length_body - i] = request[request_length - i];
	}
	body[length_body] = '\0';

	return 0;
}



/************************************************************/
/**** Test functions called each time the program start  ****/
/************************************************************/

int test_hardware() {

	printf("\n");
	printLine();
	printf("TEST : HARDWARE\n\n");

	// clignotement LED
	int time =  100;
	for(int i = 0; i< 3; i++) {
		gpio_write(1, 1);
		assert(1 == gpio_read(1));
		delay(time);
		printf("\n");

		gpio_write(1, 0);
		assert(0 == gpio_read(1));
		delay(time);
		printf("\n");
	}

	gpio_read(ID_DISTANCE_1);
	gpio_read(ID_DISTANCE_2);
	gpio_write(ID_SERVO_1, 3.14);

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
	#ifdef __APPLE__
	#else
		test_hardware();
	#endif
}





/************************************************************/
/**** SERVER                                             ****/
/************************************************************/

int main()
{
	gpio_init();

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

	    	//buff[len] = '\0';


			printf("\nReq : %s\n\n\n", buff);

			string b(buff);


			char* buff2 = (char*)malloc((MAX_SIZE)*sizeof(char));
			for(int i=0; i<MAX_SIZE; i++)
				buff2[i] = buff[i];
	    	int length_body = get_body_length(buff2);

			printf("\nReq length_body : %d\n\n\n", length_body);


	    	char* body = (char*)malloc((length_body+1)*sizeof(char));

			printf("\nReq : %s\n\n\nReceived Request (len:%u, length_body:%d)\n\n\n\n", buff, len, length_body);

	    	get_body(len, buff, length_body, body);

	    	printf("Body : %s\n", body);
	    	json_parse_body(length_body, body);


			// The new descriptor can be simply read from / written up just like a normal file descriptor
			if ( write(conn_desc, buff, len-1) > 0) {
				//printf("Written %s", buff);
			}
			else
				printf("Failed writing\n");
		}
		else
			printf("Failed receiving\n");

		printf("\n");

		// Program should always close all sockets (the connected one as well as the listening one)
		// as soon as it is done processing with it
		close(conn_desc);
	}

	close(sock_descriptor);
 
	return 0;
}
