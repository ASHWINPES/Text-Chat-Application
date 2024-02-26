/**
* @server
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
* This file contains the server init and main while loop for tha application.
* Uses the select() API to multiplex between network I/O and STDIN.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../include/server.h"
#include "../include/global.h"
#include "../include/logger.h"

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

/** 
 * References=>  https://stackoverflow.com/a/3120382 
 * https://ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/
 * */
void Server::get_my_ip(string input_command) 
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

    const char* my_ip = inet_ntop(AF_INET, &name.sin_addr, ip_buffer, 16);

	cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
    cse4589_print_and_log("IP:%s\n", my_ip);
    cse4589_print_and_log("[%s:END]\n", input_command.c_str());
}

Server::Server(string p_n) {
	port_number = p_n;
}

/**
* main function
*
* @param  argc Number of arguments
* @param  argv The argument list
* @return 0 EXIT_SUCCESS
*/
int Server::start_server(int argc, char **argv)
{
	if(argc != 3) {
		printf("Usage:%s [port]\n", argv[2]);
		exit(-1);
	}
	
	int server_socket, head_socket, selret, sock_index, caddr_len, fdaccept = 0;
	struct sockaddr_in client_addr;
	struct addrinfo hints, *res;
	fd_set master_list, watch_list;

	/* Set up hints structure */
	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

	/* Fill up address structures */
	if (getaddrinfo(NULL, argv[2], &hints, &res) != 0)
		perror("getaddrinfo failed");
	
	/* Socket */
	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(server_socket < 0)
		perror("Cannot create socket");
	
	/* Bind */
	if(bind(server_socket, res->ai_addr, res->ai_addrlen) < 0 )
		perror("Bind failed");

	freeaddrinfo(res);
	
	/* Listen */
	if(listen(server_socket, BACKLOG) < 0)
		perror("Unable to listen on port");
	
	/* ---------------------------------------------------------------------------- */
	
	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	
	/* Register the listening socket */
	FD_SET(server_socket, &master_list);
	/* Register STDIN */
	FD_SET(STDIN, &master_list);
	
	head_socket = server_socket;
	
	while(TRUE){
		memcpy(&watch_list, &master_list, sizeof(master_list));
		
		printf("\n[PA1-Server@CSE489/589]$ ");
		fflush(stdout);

		/* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");
		
		/* Check if we have sockets/STDIN to process */
		if(selret > 0){
			/* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1){
				
				if(FD_ISSET(sock_index, &watch_list)){
					
					/* Check if new command on STDIN */
					if (sock_index == STDIN){
						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
						
						memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);
						
						printf("\nI got: %s\n", cmd);
						
						//Process PA1 commands here ...
						process_server_command(cmd);
						
						free(cmd);
					}
					/* Check if new client is requesting connection */
					else if(sock_index == server_socket){
						caddr_len = sizeof(client_addr);
						fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&caddr_len);
						if(fdaccept < 0)
							perror("Accept failed.");
						
						printf("\nRemote Host connected!\n");                        
						
						/* Add to master socket list */
						FD_SET(fdaccept, &master_list);
						if(fdaccept > head_socket) head_socket = fdaccept;
					}
				}
			}
		}
	}
	
	return 0;
}

void Server::process_server_command(char* cmd) {
	string input_command(cmd);
	input_command = input_command.substr(0, input_command.size() - 1);
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
	else if (input_command == "LIST") {
		cse4589_print_and_log("[%s:SUCCESS]\n", input_command.c_str());
		map<int,pair<string,string> >::iterator itr;
		int listId = 0;
		for (itr = clients.begin(); itr != clients.end();itr++){
			cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", listId++, (itr->second).first.c_str(), (itr->second).second.c_str(), itr->first);
		}
		cse4589_print_and_log("[%s:END]\n", input_command.c_str());
	}
}