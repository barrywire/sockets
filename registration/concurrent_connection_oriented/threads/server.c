#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 5000
#define MAX_STUDENTS 100

struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

// Function to handle the clients 5 - 8
void *client_handler(void *arg)
{
    // Get client socket
    int client_socket = *((int *)arg);

    // 5. Read the data sent by client
    struct student new_student;
    recv(client_socket, (void *)&new_student, sizeof(new_student), 0);

    // Read existing data from file
    FILE *fp = fopen("registrations.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        printf("ERROR: (Server) Failed to open file in read mode\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) File opened in read mode\n");
    }

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0)
    {
        fprintf(fp, "Serial Number\tRegistration Number\tFirst Name\tLast Name\n");
    }

    // 6. Parse the data and check if student already exists
    // 6.1 Create a new student struct
    struct student students[MAX_STUDENTS];
    int num_students = 0;

    while (fread(&students[num_students], sizeof(struct student), 1, fp))
    {
        num_students++;
    }

    fclose(fp);

    // 6.2 Check if student already exists
    int i, student_exists = 0;
    for (i = 0; i < num_students; i++)
    {
        if (new_student.serial_number == students[i].serial_number)
        {
            student_exists = 1;
            break;
        }

        if (strcmp(new_student.reg_number, students[i].reg_number) == 0)
        {
            student_exists = 1;
            break;
        }
    }

    // 7. If student does not exist, add to array and file
    if (!student_exists)
    {
        // 7.1 Add new student to array
        students[num_students] = new_student;
        num_students++;

        // 7.1 Write updated data to file
        fp = fopen("registrations.txt", "a+");
        if (fp == NULL)
        {
            perror("Error opening file");
            printf("ERROR: (Server) Failed to open file in append mode\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("INFO: (Server) File opened in append mode to write the data\n");
        }

        for (int i = 0; i < num_students; i++)
        {
            fprintf(fp, "%d\t\t\t\t%s\t\t\t%s\t%s\n", students[i].serial_number, students[i].reg_number, students[i].first_name, students[i].last_name);
        }

        fclose(fp);

        // 8. Send success message
        char success_message[] = "INFO: (Server) Registration successful!";
        send(client_socket, success_message, strlen(success_message), 0);
    }
    else
    {
        // Send error message
        char error_message[] = "ERROR: (Server) Registration failed: Serial number or registration number already exists!";
        send(client_socket, error_message, strlen(error_message), 0);
    }

    printf("INFO: (Server) Client disconnected\n");

    close(client_socket);
    pthread_exit(NULL);
}

int main()
{
    // 1. Create a socket for the server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        printf("ERROR: (Server) Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket created successfully - %d\n", server_socket);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // 2. Bind the socket to a specific port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket");
        printf("ERROR: (Server) Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket bound successfully\n");
    }

    // 3. Listen for incoming connections. Maximum 5 connections can be queued.
    if (listen(server_socket, 5) < 0)
    {
        perror("Error listening on socket");
        printf("ERROR: (Server) Failed to listen on socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket listening successfully\n");
    }

    printf("Server started. Listening on port %d\n", SERVER_PORT);

    while (1)
    {
        // 4. Accept incoming connection
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            perror("Error accepting client connection");
            printf("ERROR: (Server) Failed to accept client connection\n");
            continue;
        }
        else
        {
            printf("INFO: (Server) Client connected\n");
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)&client_socket) != 0)
        {
            perror("Error creating thread");
            printf("ERROR: (Server) Failed to create thread\n");
            continue;
        }
        else
        {
            printf("INFO: (Server) Thread created successfully\n");
        }

        printf("New client connected. Thread created with ID: %ld\n", tid);
    }

    // 9. Close the connection.
    close(server_socket);
    return 0;
}
