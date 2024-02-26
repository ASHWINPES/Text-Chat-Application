/**
* @client
* @author  Swetank Kumar Saha <swetankk@buffalo.edu>, Shivang Aggarwal <shivanga@buffalo.edu>
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details at
* http://www.gnu.org/copyleft/gpl.html
*
* @section DESCRIPTION
*
* This file contains the client.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include "../include/client.h"
#include "../include/global.h"
#include "../include/logger.h"

using namespace std;

#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256

/** 
 * References=>  https://stackoverflow.com/a/3120382 
 * https://ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/
 * */
void Client::get_my_ip(string input_command) 
{
	char* ip_buffer;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    const char* google_dns = "8.8.8.8";
    uint16_t google_dns_port = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns);
    serv.sin_port = htons(google_dns_port);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);

    my_ip = inet_ntop(AF_INET, &name.sin_addr, ip_buffer, 16);

	cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
    cse4589_print_and_log("IP:%s\n", my_ip);
    cse4589_print_and_log("[%s:END]\n", input_command.c_str());
}

Client::Client(string p_n) {
    port_number = p_n;
}

int Client::start_client(int argc, char **argv)
{
	if(argc != 3) {
		printf("Usage:%s [ip] [port]\n", argv[0]);
		exit(-1);
	}
	
	while(TRUE){
		printf("\n[PA1-Client@CSE489/589]$ ");
		fflush(stdout);
		
		char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
		memset(msg, '\0', MSG_SIZE);
		if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
			exit(-1);
		
		printf("I got: %s(size:%lu chars)\n", msg, strlen(msg));

        string input_command(msg);
        input_command = input_command.substr(0, input_command.size() - 1);
        // c_str is used to avoid this error => 
        // Reference https://stackoverflow.com/a/23450339 
        if (input_command == "AUTHOR") {
            cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
            cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "smritira");
            cse4589_print_and_log("[%s:END]\n", input_command.c_str());
        }
        else if (input_command == "IP") {
			get_my_ip(input_command);
        }
        else if (input_command == "PORT") {
            cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
            cse4589_print_and_log("PORT:%s\n", port_number.c_str());
            cse4589_print_and_log("[%s:END]\n", input_command.c_str());
        }
        else if (input_command == "EXIT") {
            cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
            cse4589_print_and_log("[%s:END]\n", input_command.c_str());
            exit(0);
        }
        else if (input_command == "LOGIN") {
			if(argc==3){
				string serverIP(argv[1]);
				int port = stoi(argv[2]);
				struct sockaddr_in sa;
				int isValidIP = inet_pton(AF_INET, serverIP.c_str(), &(sa.sin_addr));
				if(isValidIP != 1 || (port < 1024 || port > 65535)){
					cse4589_print_and_log("[%s:ERROR]\n", input_command.c_str());
					cse4589_print_and_log("[%s:END]\n", input_command.c_str());			
				}
				cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
				int server;
				server = connect_to_host(argv[1], argv[2]);
				cse4589_print_and_log("[%s:END]\n", input_command.c_str());
			}
			else {
				cse4589_print_and_log("[%s:ERROR]\n", input_command.c_str());
				cse4589_print_and_log("[%s:END]\n", input_command.c_str());
			}

        }
		else if (input_command == "LIST") {
			cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
			map<int,pair<string,string> >::iterator itr;
			int listId = 0;
			for (itr = clients.begin(); itr != clients.end();itr++){
				cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", listId++, (itr->second).first.c_str(), (itr->second).second.c_str(), itr->first);
			}
			cse4589_print_and_log("[%s:END]\n", input_command.c_str());
		}
		else if (input_command == "REFRESH") {
			cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
			map<int,pair<string,string> >::iterator itr;
			int listId = 0;
			for (itr = clients.begin(); itr != clients.end();itr++){
				cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", listId++, (itr->second).first.c_str(), (itr->second).second.c_str(), itr->first);
			}
			cse4589_print_and_log("[%s:END]\n", input_command.c_str());
		}
		
		// printf("\nSENDing it to the remote server ... ");
		// if(send(server, msg, strlen(msg), 0) == strlen(msg))
		// 	printf("Done!\n");
		fflush(stdout);
		
		/* Initialize buffer to receieve response */
		char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		memset(buffer, '\0', BUFFER_SIZE);
		
		// if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
		// 	printf("Server responded: %s", buffer);
		// 	fflush(stdout);
		// }
	}

}

int Client::connect_to_host(char *server_ip, char* server_port)
{
	int fdsocket;
	struct addrinfo hints, *res;

	/* Set up hints structure */	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Fill up address structures */	
	if (getaddrinfo(server_ip, server_port, &hints, &res) != 0)
		perror("getaddrinfo failed");

	/* Socket */
	fdsocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fdsocket < 0)
		perror("Failed to create socket");
	
	/* Connect */
	int connret = connect(fdsocket, res->ai_addr, res->ai_addrlen);
	if(connret < 0)
		perror("Connect failed");
	char *hostname;
	gethostname(hostname, 1023);

	int port = stoi(port_number);
	string myIP(my_ip);
	string myName(hostname);

	clients[port] = make_pair(myName, myIP);
	
	freeaddrinfo(res);

	return fdsocket;
}