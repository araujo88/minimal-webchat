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
#include <signal.h>  // for interrupt signal handler
#include <pthread.h> // POSIX threads
#include "../include/common.h"

int server_socket; // global variable in order to be handled after SIGINT
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Error - IP address not provided\n");
        exit(EXIT_FAILURE);
    }
    if (argc < 3) {
        printf("Error - port not provided\n");
        exit(EXIT_FAILURE);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    pthread_mutex_init(&mutex, 0);

    printf("IP address: %s - port: %d\n", ip, port);

    puts("Creating socket ...");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    check_socket(server_socket);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;            // specifies protocol IPv4
    server_address.sin_port = htons(port);          // specifies port (host to network)
    server_address.sin_addr.s_addr = inet_addr(ip); // converts IP string to standard IPv4 decimal notation
    // server_address.sin_addr.s_addr = INADDR_ANY;

    puts("Binding socket ...");
    check_bind(server_socket, &server_address);
    puts("Binding done!");

    int client_socket;
    struct sockaddr_in *client_address = NULL;

    check_listen(server_socket, 10);
    check_accept(server_socket, &client_socket, (struct sockaddr *)client_address);

    signal(SIGINT, handle_signal);
    while (true)
    {
        pthread_t t1, t2;

        pthread_create(&t1, NULL, send_message, (void *)&client_socket);
        pthread_create(&t2, NULL, recv_message, (void *)&client_socket);
        pthread_join(t1, NULL);
    }
    return 0;
}

void handle_signal(int sig)
{
    printf("\nCaught interrupt signal %d\n", sig);
    printf("Terminating threads\n");
    pthread_mutex_destroy(&mutex);
    // closes the socket
    puts("Closing socket ...");
    if (close(server_socket) == 0)
    {
        puts("Socket closed!");
        exit(EXIT_SUCCESS);
    }
    else
    {
        perror("An error occurred while closing the socket: ");
        printf("Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}
