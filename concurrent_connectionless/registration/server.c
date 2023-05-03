#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

void *handle_packet(void *arg);

int main(int argc, char *argv[])
{
    int server_socket, client_socket, address_length, recv_size;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];
    struct student students[MAX_CLIENTS];
    int student_count = 0;
    FILE *file;

    // Create socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
    {
        printf("ERROR: (Server) Could not create socket\n");
        return 1;
    }
    else
    {
        printf("INFO: (Server) Created a socket successfully - %d\n", server_socket);
    }

    // Bind socket to port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        printf("ERROR: (Server) Could not bind socket to port\n");
        return 1;
    }
    else
    {
        printf("INFO: (Server) Socket bound to port %d\n", ntohs(server_address.sin_port));
    }

    // Check if file is empty and write header line
    file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        perror("File open failed");
        printf("ERROR: (Server) Could not open file for comparison of registration and serial numbers\n");
        return 1;
    }
    else
    {
        printf("INFO: (Server) Opened file successfully for comparison of registration and serial numbers\n");
    }

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
    {
        fprintf(file, "Serial Number\tRegistration Number\tFirst Name\tLast Name\n");
    }

    fclose(file);

    // Receive data from clients and save to file
    while (1)
    {
        printf("Waiting for client data...\n");

        // Receive data from client
        address_length = sizeof(client_address);
        memset(buffer, 0, BUFFER_SIZE);
        recv_size = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &address_length);

        if (recv_size < 0)
        {
            perror("Receive failed");
            printf("ERROR: (Server) Could not receive data from client\n");
            continue;
        }
        else
        {
            printf("INFO: (Server) Received data from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        }

        printf("INFO: (Server) Data received from %s:%d: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);

        // Create a new thread to handle the packet
        pthread_t tid;
        int *new_sock = malloc(sizeof(int));
        *new_sock = server_socket;
        struct sockaddr_in *client = malloc(sizeof(struct sockaddr_in));
        memcpy(client, &client_address, sizeof(client_address));
        if (pthread_create(&tid, NULL, handle_packet, (void *)new_sock) < 0)
        {
            perror("Thread creation failed");
            printf("ERROR: (Server) Could not create thread to handle packet\n");
            continue;
        }
        else
        {
            printf("INFO: (Server) Created thread %lu to handle packet\n", tid);
        }
    }
    return 0;
}

void *handle_packet(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int recv_size;

    // Receive data from client
    memset(buffer, 0, BUFFER_SIZE);
    recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (recv_size < 0)
    {
        perror("Receive failed");
        printf("ERROR: (Server) Could not receive data from client\n");
        return NULL;
    }
    else
    {
        printf("INFO: (Server) Received data from client: %s\n", buffer);
    }

    // Parse the comma-separated data received from the client
    char *token;
    struct student s;

    token = strtok(buffer, ",");
    s.serial_number = atoi(token);

    token = strtok(NULL, ",");
    strncpy(s.reg_number, token, sizeof(s.reg_number) - 1);

    token = strtok(NULL, ",");
    strncpy(s.first_name, token, sizeof(s.first_name) - 1);

    token = strtok(NULL, ",");
    strncpy(s.last_name, token, sizeof(s.last_name) - 1);

    // Save the student data to file
    FILE *file = fopen("registrations.txt", "a");
    if (file == NULL)
    {
        perror("File open failed");
        printf("ERROR: (Server) Could not open file for writing\n");
        return NULL;
    }

    fprintf(file, "%d\t%s\t%s\t%s\n", s.serial_number, s.reg_number, s.first_name, s.last_name);
    fclose(file);

    // Send a response back to the client
    char response[] = "Data received and saved to file.";
    if (send(client_socket, response, strlen(response), 0) < 0)
    {
        perror("Send failed");
        printf("ERROR: (Server) Could not send response to client\n");
    }
    else
    {
        printf("INFO: (Server) Response sent to client: %s\n", response);
    }

    return NULL;
}
