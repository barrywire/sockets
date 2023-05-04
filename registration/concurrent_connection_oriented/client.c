#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000

struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

int main()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        printf("ERROR: (Client) Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Client) Socket created - %d\n", client_socket);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to server");
        printf("ERROR: (Client) Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Client) Connected to server\n");
    }

    struct student student_data;
    printf("Enter student's serial number: ");
    scanf("%d", &student_data.serial_number);
    printf("Enter student's registration number: ");
    scanf("%s", student_data.reg_number);
    printf("Enter student's first name: ");
    scanf("%s", student_data.first_name);
    printf("Enter student's last name: ");
    scanf("%s", student_data.last_name);

    if (send(client_socket, (void *)&student_data, sizeof(student_data), 0) < 0)
    {
        perror("Error sending data");
        printf("ERROR: (Client) Failed to send data\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Client) Data sent successfully\n");
    }

    char server_response[1024];
    if (recv(client_socket, server_response, 1024, 0) < 0)
    {
        perror("Error receiving data");
        printf("ERROR: (Client) Failed to receive data\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Client) Data received successfully\n");
    }

    printf("Server response:\n%s\n", server_response);

    close(client_socket);
    return 0;
}
