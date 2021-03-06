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
#include <cstring>
#include <sstream>
#include <time.h>
#include <math.h>
#include <iostream>
#include <thread>

// Jsons
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/prettywriter.h"
#include "rapidjson/include/rapidjson/filestream.h"
#include <cstdio>

// Local files
#include "utils.h"
#include "hardware.h"


// Server
#define MAX_SIZE 900000
#define PORT 8888


static double* HARDWARE_DISTANCE;
static const int HARDWARE_DISTANCE_SIZE = 2;

using std::string;


/************************************************************/
/**** Create response                                    ****/
/************************************************************/

string create_response_hardware(int id, string type, double value) {
	std::stringstream ss("");
	ss << "{\"id\":";
	ss << id;
	ss << ",\"read\":true,\"type\":\"";
	ss << type;
	ss << "\",\"suceed\":true,\"value\":\"";
    ss << value;
    ss << "\"}";
    return ss.str();
}

string create_response_hardware(int id, string type) {
	return create_response_hardware(id, type, hardware_read(id));
}

string create_response_hardware_distance() {	
	std::stringstream ss("");
	ss << create_response_hardware(ID_DISTANCE_1, "distance", HARDWARE_DISTANCE[0]);
	ss << ",";
	ss << create_response_hardware(ID_DISTANCE_2, "distance", HARDWARE_DISTANCE[1]);
	return ss.str();
}

string create_response() {

	std::stringstream ss("");
	ss << "{\"succeed\":true,\"content\":{\"hardware\":[";
    ss << create_response_hardware(ID_LED_1, "led");
    ss << ",";
    ss << create_response_hardware_distance();
    ss << "]}}";

	string body = ss.str();

	std::stringstream response("");
	response << "HTTP/1.0 200 Document follows\r\nServer: RaspberryPi \r\nContent-Type: application/json\r\nContent-Length: ";
	response << body.size() << "\r\n\r\n" << body;

    return response.str();
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
		printf("Error JSON parsing (1)\n");
		return NULL;
	}

	rapidjson::Document json;
	
	if (json.Parse<0>(body).HasParseError()) {
		printf("Error JSON parsing (2)\n");
		return NULL;
	}

	if(json.HasMember("succeed")) {
		if(json["succeed"].IsBool()) {
			bool succeed = json["succeed"].GetBool();
		}
    }

    if(json.HasMember("toast")) {
    	string toast = json["toast"].GetString();
    }

    if(json.HasMember("debug")) {
    	string debug = json["debug"].GetString();
    }   

    if(json.HasMember("content")) {

    	const rapidjson::Value& content = json["content"];

    	if(content.HasMember("date_request")) {
	    	string date_request = content["date_request"].GetString();
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

									hardware_read(current_hardware_id);

								}
								else {

									if(current_hardware.HasMember("value")) {

										string value = current_hardware["value"].GetString();
								    	hardware_write(current_hardware_id, atof(value.c_str()));
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
	string request_str(request);

	if(request_str.find("\r\n\r\n") != std::string::npos) {
		memcpy(body,strstr(request,"\r\n\r\n"), length_body+4);
		body[length_body+4] = '\0';
	}
	return 0;
}



/************************************************************/
/**** Test functions called each time the program start  ****/
/************************************************************/

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
		hardware_test();
	#endif
}



//The function we want to make the thread run.
void task_ai(string name, bool debug)
{
	int id_loop = 0;

	while(1) {

		if(debug) {
			printf("#%d [%s]\n", id_loop, name.c_str());
		}

		HARDWARE_DISTANCE = hardware_read_distance();
		
		id_loop++;
		tempo(200);
	}
    
}

void launch_thread()
{
	// Constructs the new thread and runs it. Does not block execution.
    std::thread t1(task_ai, "ai", false);

    //Makes the main thread wait for the new thread to finish execution, therefore blocks its own execution.
    //t1.join();

    t1.detach();
}



/************************************************************/
/**** SERVER                                             ****/
/************************************************************/

int main()
{
	HARDWARE_DISTANCE = new double[HARDWARE_DISTANCE_SIZE];

	if(hardware_init() < 0) {
		printf("Error hardware_init() < 0");
		return 0;
	}

	test();

	launch_thread();

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

			//printf("\nReq : %s\n\n\n", buff);

			string b(buff);


			char* buff2 = (char*)malloc((MAX_SIZE)*sizeof(char));
			for(int i=0; i<MAX_SIZE; i++)
				buff2[i] = buff[i];
	    	int length_body = get_body_length(buff2);
	    	free(buff2);

			//printf("\nReq length_body : %d\n\n\n", length_body);


	    	char* body = (char*)malloc((length_body+4+1)*sizeof(char));

			//printf("\nReq : %s\n\n\nReceived Request (len:%u, length_body:%d)\n\n", buff, len, length_body);

	    	get_body(len, buff, length_body, body);

	    	//printf("Body : %s\n\n", body);
	    	json_parse_body(length_body, body);


	    	string response = create_response();

			// The new descriptor can be simply read from / written up just like a normal file descriptor
			if ( write(conn_desc, response.c_str(), response.size()) > 0) {
				//printf("Written %s", buff);
			}
			else
				printf("Failed writing\n");

			free(body);
		}
		else
			printf("Failed receiving\n");

		printf("\n");

		// Program should always close all sockets (the connected one as well as the listening one)
		// as soon as it is done processing with it
		close(conn_desc);

		memset(&buff[0], 0, MAX_SIZE);
	}

	close(sock_descriptor);
 
	return 0;
}
