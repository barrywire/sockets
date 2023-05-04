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

void *client_handler(void *arg)
{
    int server_socket = *((int *)arg);

    struct student new_student;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    recvfrom(server_socket, (void *)&new_student, sizeof(new_student), 0, (struct sockaddr *)&client_addr, &client_addr_len);

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

    struct student students[MAX_STUDENTS];
    int num_students = 0;

    while (fread(&students[num_students], sizeof(struct student), 1, fp))
    {
        num_students++;
    }

    fclose(fp);

    // Check if student already exists
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

    // If student does not exist, add to array and file
    if (!student_exists)
    {
        // Add new student to array
        students[num_students] = new_student;
        num_students++;

        // Write updated data to file
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

        // Send success message
        char success_message[] = "INFO: (Server) Registration successful!";
        sendto(server_socket, success_message, strlen(success_message), 0, (struct sockaddr *)&client_addr, client_addr_len);
    }
    else
    {
        // Send error message
        char error_message[] = "ERROR: (Server) Registration failed: Serial number or registration number already exists!";
        sendto(server_socket, error_message, strlen(error_message), 0, (struct sockaddr *)&client_addr, client_addr_len);
    }

    printf("INFO: (Server) Client handler thread exiting...\n");

    // Close socket
    close(server_socket);

    return NULL;
}

int main()
{
    // Create server socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
    {
        perror("Error creating socket");
        printf("ERROR: (Server) Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket created successfully\n");
    }

    // Bind socket to port
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket");
        printf("ERROR: (Server) Failed to bind socket to port %d\n", SERVER_PORT);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket bound to port %d successfully\n", SERVER_PORT);
    }

    // Listen for connections
    printf("INFO: (Server) Listening for incoming connections...\n");

    while (1)
    {
        // Receive data from client
        struct student new_student;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        recvfrom(server_socket, (void *)&new_student, sizeof(new_student), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        // Create client handler thread
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void *)&server_socket) != 0)
        {
            perror("Error creating thread");
            printf("ERROR: (Server) Failed to create client handler thread\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("INFO: (Server) Client handler thread created successfully\n");
        }

        // Detach thread
        pthread_detach(thread_id);
    }

    return 0;
}
