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
pthread_mutex_t mutex;

void handle_signal(int sig);                                                                   // interrupt signal handler
void check_socket(int socket);                                                                 // checks socket
void check_connection(int socket, struct sockaddr_in6 address);                                // checks connection
void check_bind(int server_socket, struct sockaddr_in6 *server_address);                       // check binding
void check_listen(int server_socket, int number_connections);                                  // check connection listening
void check_accept(int server_socket, int *client_socket, struct sockaddr_in6 *client_address); // check accepting connection
void *send_message(void *socket);                                                              // sends message
void *recv_message(void *socket);                                                              // receives message

int main(int argc, char *argv[])
{
    char *ip = argv[1];
    int port = atoi(argv[2]);

    pthread_mutex_init(&mutex, 0);

    printf("IP address: %s - port: %d\n", ip, port);

    puts("Creating socket ...");
    server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    check_socket(server_socket);

    struct sockaddr_in6 server_address;
    server_address.sin6_family = AF_INET6;  // specifies protocol IPv6
    server_address.sin6_port = htons(port); // specifies port (host to network)
    server_address.sin6_addr = in6addr_any;
    // inet_pton(AF_INET6, ip, &(server_address.sin6_addr));

    puts("Binding socket ...");
    check_bind(server_socket, &server_address);
    puts("Binding done!");

    signal(SIGINT, handle_signal);
    while (true)
    {
        int *client_socket = (int *)malloc(sizeof(int));
        struct sockaddr_in6 client_address;

        check_listen(server_socket, 1);
        check_accept(server_socket, client_socket, (struct sockaddr_in6 *)&client_address);
        pthread_create(&t1, NULL, send_message, (void *)client_socket);
        pthread_create(&t2, NULL, recv_message, (void *)client_socket);
    }
    return 0;
}

void handle_signal(int sig)
{
    printf("\nCaught interrupt signal %d\n", sig);
    printf("Terminating threads\n");
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
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

void check_connection(int socket, struct sockaddr_in6 address)
{
    if ((connect(socket, (struct sockaddr_in6 *)&address, sizeof(address))) < 0)
    {
        perror("Connection error");
        printf("Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    puts("Connected!");
}

void check_bind(int server_socket, struct sockaddr_in6 *server_address)
{
    if ((bind(server_socket, (struct sockaddr *)server_address, sizeof(*server_address))) < 0)
    {
        perror("Bind failed");
        printf("Error code: %d\n", errno);
        exit(1);
    }
}

void check_listen(int server_socket, int num_connections)
{
    if ((listen(server_socket, num_connections)) < 0)
    {
        perror("Listen failed");
        printf("Error code: %d\n", errno);
        exit(1);
    }
}

void check_accept(int server_socket, int *client_socket, struct sockaddr_in6 *client_address)
{
    if ((*client_socket = accept(server_socket, (struct sockaddr *)client_address, (socklen_t *)sizeof(client_address))) < 0)
    {
        perror("Accept failed");
        printf("Error code: %d\n", errno);
        exit(1);
    }
}

void *send_message(void *client_socket)
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

    pthread_mutex_lock(&mutex);
    send(*(int *)client_socket, buffer, strlen(buffer), 0); // sends message
    printf("Sent: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    free(client_socket);
    client_socket = NULL;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *recv_message(void *client_socket)
{
    char buffer[BUFFER_SIZE];

    pthread_mutex_lock(&mutex);
    recv(*(int *)client_socket, &buffer, sizeof(buffer), 0); // receives message
    printf("Received: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    free(client_socket);
    client_socket = NULL;
    pthread_mutex_unlock(&mutex);

    return NULL;
}