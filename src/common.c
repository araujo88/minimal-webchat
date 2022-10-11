#include "../include/common.h"

void *send_message(void *client_socket)
{
    char buffer[BUFFER_SIZE];
    char *date;
    char message[MESSAGE_SIZE];
    time_t t; // define response date

    time(&t);
    date = ctime(&t);
    date[strcspn(date, "\n")] = 0; // removes newline for correct parsing

    puts("Type a message ...");
    fgets(message, MESSAGE_SIZE, stdin);
    //scanf("%s", message);
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
