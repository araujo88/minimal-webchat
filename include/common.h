#ifndef _COMMON_H
#define _COMMON_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // close socket
#include <errno.h>   // error codes
#include <stdbool.h> // boolean types
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
#include <time.h>
#include <pthread.h> // POSIX threads

#define BUFFER_SIZE 65536
#define MESSAGE_SIZE 8192

void handle_signal(int sig);                                                               // interrupt signal handler
void check_socket(int socket);                                                             // checks socket
void check_connection(int socket, struct sockaddr_in address);                             // checks connection
void check_bind(int server_socket, struct sockaddr_in *server_address);                    // check binding
void check_listen(int server_socket, int number_connections);                              // check connection listening
void check_accept(int server_socket, int *client_socket, struct sockaddr *client_address); // check accepting connection
void *send_message(void *socket);                                                          // sends message
void *recv_message(void *socket);                                                          // receives message
void send_message2(void *socket);                                                          // sends message
void recv_message2(void *socket);                                                          // receives message

#endif