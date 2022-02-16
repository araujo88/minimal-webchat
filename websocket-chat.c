#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // close socket
#include <errno.h>   // error codes
#include <stdbool.h> // boolean types
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <signal.h>  // for interrupt signal handler
#include <pthread.h> // POSIX threads

#define BUFFER_SIZE 1024

int server_socket; // global variable in order to be handled after SIGINT
pthread_t t1, t2;

void handle_signal(int sig);                                   // interrupt signal handler
void check_socket(int socket);                                 // checks socket
void check_connection(int socket, struct sockaddr_in address); // checks connection
void *send_message(void *socket);                              // sends message
void *recv_message(void *socket);                              // receives message

int main(int argc, char *argv[])
{
    char *ip = argv[1];
    int port = atoi(argv[2]);

    printf("IP address: %s - port: %d\n", ip, port);

    puts("Creating socket ...");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    check_socket(server_socket);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;         // specifies protocol IPv4
    server_address.sin_port = htons(port);       // specifies port (host to network)
    server_address.sin_addr.s_addr = INADDR_ANY; // converts IP string to standard IPv4 decimal notation

    check_connection(server_socket, server_address);

    signal(SIGINT, handle_signal);
    while (true)
    {
        pthread_create(&t1, NULL, send_message, (void *)&server_socket);
        pthread_create(&t2, NULL, recv_message, (void *)&server_socket);
    }
    return 0;
}

void handle_signal(int sig)
{
    printf("\nCaught interrupt signal %d\n", sig);
    printf("Terminating threads\n");
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
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

void check_socket(int socket)
{
    if (socket < 0)
    {
        perror("Socket failed: ");
        printf("Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    puts("Socket created!");
}

void check_connection(int socket, struct sockaddr_in address)
{
    if ((connect(socket, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        perror("Connection error");
        printf("Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    puts("Connected!");
}

void *send_message(void *socket)
{
    char buffer[BUFFER_SIZE];
    char *date;
    char message[BUFFER_SIZE / 2];
    time_t t; // define response date
    time(&t);

    date = ctime(&t);
    date[strcspn(date, "\n")] = 0; // removes newline for correct parsing

    scanf("%s", message);
    sprintf(buffer, "%s - %s", date, message);
    send(*(int *)socket, buffer, strlen(buffer), 0); // sends message
    printf("Sent: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    return NULL;
}

void *recv_message(void *socket)
{
    char buffer[BUFFER_SIZE];

    recv(*(int *)socket, &buffer, sizeof(buffer), 0); // receives message
    printf("Received: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    return NULL;
}