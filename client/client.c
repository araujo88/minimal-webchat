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

#define BUFFER_SIZE 65536

int server_socket; // global variable in order to be handled after SIGINT
pthread_mutex_t mutex;

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

int main(int argc, char *argv[])
{
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
    server_address.sin_addr.s_addr = inet_addr(ip); // converts IP string to standard IPv4 decimal notation INADDR_ANY
    // server_address.sin_addr.s_addr = INADDR_ANY;

    connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    signal(SIGINT, handle_signal);
    while (true)
    {
        pthread_t t1, t2;

        pthread_create(&t1, NULL, send_message, (void *)&server_socket);
        pthread_create(&t2, NULL, recv_message, (void *)&server_socket);
        pthread_join(t1, NULL);
    }
    return 0;
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

    puts("Type a message ...");
    scanf("%s", message);
    sprintf(buffer, "%s - %s", date, message);
    send(*(int *)client_socket, buffer, strlen(buffer), 0); // sends message
    printf("\nSent: %s\n\n", buffer);

    return NULL;
}

void *recv_message(void *client_socket)
{
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, sizeof(buffer));
    if (recv(*(int *)client_socket, &buffer, sizeof(buffer), 0) > 0) // receives message
    {
        printf("Received: %s\n\n", buffer);
    }

    return NULL;
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

void check_bind(int server_socket, struct sockaddr_in *server_address)
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

void check_accept(int server_socket, int *client_socket, struct sockaddr *client_address)
{
    if ((*client_socket = accept(server_socket, (struct sockaddr *)client_address, (socklen_t *)sizeof(client_address))) < 0)
    {
        perror("Accept failed");
        printf("Error code: %d\n", errno);
        exit(1);
    }
}
