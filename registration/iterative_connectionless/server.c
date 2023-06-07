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
                printf("ERROR: (Server) Registration failed: Student already registered or serial number already taken\n");

                // Send the response to the client
                char response[BUFFER_SIZE];
                strcpy(response, "ERROR: (Server) Registration failed: Student already registered or serial number already taken");
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
                printf("ERROR: (Server) Could not open file to save the data to file\n");
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

            printf("INFO: (Server) Registration successful\n");
        }

        // Send response to client
        char response[BUFFER_SIZE];
        if (i == student_count - 1)
        {
            strcpy(response, "INFO: (Server) Registration successful");
        }
        else
        {
            strcpy(response, "ERROR: (Server) Registration failed");
        }

        if (sendto(server_socket, response, strlen(response), 0, (struct sockaddr *)&client_address, address_length) < 0)
        {
            perror("Send failed");
            printf("ERROR: (Server) Could not send response to client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        }
    }

    close(server_socket);

    return 0;
}