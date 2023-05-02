#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

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
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        printf("ERROR: (Server) Could not listen for incoming connections\n");
        return 1;
    }
    else
    {
        printf("INFO: (Server) Listening for incoming connections...\n");
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

    // Accept incoming connections and receive data from clients
    while (1)
    {
        printf("Waiting for incoming connections...\n");

        // Accept incoming connection
        address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&address_length);

        if (client_socket < 0)
        {
            perror("Accept failed");
            printf("ERROR: (Server) Could not accept incoming connection\n");
            continue;
        }
        else
        {
            printf("INFO: (Server) Accepted incoming connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        }

        // Receive data from client
        memset(buffer, 0, BUFFER_SIZE);
        recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (recv_size < 0)
        {
            perror("Receive failed");

            printf("ERROR: (Server) Could not receive data from client\n");
            continue;
        }
        else if (recv_size == 0)
        {
            printf("INFO: (Server) Connection closed by client\n");
            continue;
        }
        else
        {
            // Parse received data and store in struct
            struct student new_student;
            char *ptr = strtok(buffer, ",");
            new_student.serial_number = atoi(ptr);
            ptr = strtok(NULL, ",");
            strcpy(new_student.reg_number, ptr);
            ptr = strtok(NULL, ",");
            strcpy(new_student.first_name, ptr);
            ptr = strtok(NULL, ",");
            strcpy(new_student.last_name, ptr);
            // Check if student already exists
            int i, student_exists = 0;
            for (i = 0; i < student_count; i++)
            {
                if (strcmp(students[i].reg_number, new_student.reg_number) == 0)
                {
                    student_exists = 1;
                    break;
                }
            }

            // If student does not exist, add to array and file
            if (!student_exists)
            {
                students[student_count] = new_student;
                student_count++;

                file = fopen("registrations.txt", "a");
                if (file == NULL)
                {
                    perror("File open failed");
                    printf("ERROR: (Server) Could not open file for writing\n");
                    return 1;
                }
                else
                {
                    printf("INFO: (Server) Opened file successfully for writing\n");
                }

                fprintf(file, "%d\t\t\t\t%s\t\t\t%s\t%s\n", new_student.serial_number, new_student.reg_number, new_student.first_name, new_student.last_name);

                fclose(file);

                printf("INFO: (Server) Student added successfully\n");
            }
            else
            {
                printf("INFO: (Server) Student already exists\n");
            }

            // Send response to client
            char response[2048];
            if (student_exists)
            {
                sprintf(response, "Student with registration number %s already exists", new_student.reg_number);
            }
            else
            {
                sprintf(response, "Student with registration number %s added successfully", new_student.reg_number);
            }

            send(client_socket, response, strlen(response), 0);

            // Close connection with client
            close(client_socket);
            printf("INFO: (Server) Connection closed with %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        }
    }

    return 0;
}