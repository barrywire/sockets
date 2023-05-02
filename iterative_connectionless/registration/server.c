#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

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
        printf("Could not create socket\n");
        return 1;
    }

    // Bind socket to port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    // Check if file is empty and write header line
    file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        perror("File open failed");
        return 1;
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
            continue;
        }

        printf("Received data from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        printf("Data: %s\n", buffer);

        // Parse data and check for uniqueness
        char *token = strtok(buffer, ",");
        int serial_number = atoi(token);
        token = strtok(NULL, ",");
        char reg_number[20];
        strcpy(reg_number, token);
        token = strtok(NULL, ",");
        char first_name[50];
        strcpy(first_name, token);
        token = strtok(NULL, ",");
        char last_name[50];
        strcpy(last_name, token);

        int i;
        for (i = 0; i < student_count; i++)
        {
            if (strcmp(reg_number, students[i].reg_number) == 0 || serial_number == students[i].serial_number)
            {
                printf("Registration failed: Student already registered or serial number already taken\n");
                break;
            }
        }
        if (i == student_count)
        {
            // Save student data to file
            file = fopen("registrations.txt", "a");
            if (file == NULL)
            {
                perror("File open failed");
                continue;
            }

            fprintf(file, "%d\t\t\t\t%s\t\t\t%s\t%s\n", serial_number, reg_number, first_name, last_name);
            fclose(file);

            // Save student data to array
            students[student_count].serial_number = serial_number;
            strcpy(students[student_count].reg_number, reg_number);
            strcpy(students[student_count].first_name, first_name);
            strcpy(students[student_count].last_name, last_name);

            student_count++;

            printf("Registration successful\n");
        }

        // Send response to client
        char response[BUFFER_SIZE];
        if (i == student_count - 1)
        {
            strcpy(response, "Registration successful");
        }
        else
        {
            strcpy(response, "Registration failed");
        }

        if (sendto(server_socket, response, strlen(response), 0, (struct sockaddr *)&client_address, address_length) < 0)
        {
            perror("Send failed");
        }
    }

    close(server_socket);

    return 0;
}