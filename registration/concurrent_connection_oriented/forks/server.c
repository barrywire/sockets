#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 5000
#define MAX_STUDENTS 100

// Conceptual algorithm to follow for a concurrent connection-oriented server that uses the fork system call:
// 1. Create a socket with the socket() system call.
// 2. Bind the socket to an address using the bind() system call.
// 3. Listen for connections with the listen() system call. Have a maximum number of connections that can be queued - 5.
// 4. Continually accept incoming connections with clients using the accept() system call.
// 5. Fork a child process to handle each client connection.
// 6. In the child process, receive data from the client using the recv() system call.
// 7. Parse the data and check if the student already exists
// 8. If the student does not exist, save the student details to a file.
// 9. Formulate and send a response message based on the result of the data processing using the send() system call.
// 10. Close the child process socket and exit.
// 11. In the parent process, close the client socket and continue listening for incoming connections.

struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

// Function to handle the clients 6 - 10
void *client_handler(void *arg)
{
    // 6. In the child process, receive data from the client using the recv() system call
    // Get client socket
    int client_socket = *((int *)arg);

    // Read the data sent by client
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

    // 7. Parse the data and check if student already exists
    // 7.1 Create a new student struct
    struct student students[MAX_STUDENTS];
    int num_students = 0;

    while (fread(&students[num_students], sizeof(struct student), 1, fp))
    {
        num_students++;
    }

    fclose(fp);

    // 7.2 Check if student already exists
    int i, student_exists = 0;
    for (i = 0; i < num_students; i++)
    {
        if (new_student.serial_number == students[i].serial_number)
        {
            student_exists = 1;
            break;
        }

        if (strcmp(students[i].reg_number, new_student.reg_number) == 0)
        {
            student_exists = 1;
            break;
        }
    }

    // 8. If the student does not exist, save the student details to a file
    if (!student_exists)
    {
        // 8.1 Add new student to array
        students[num_students] = new_student;
        num_students++;

        // 8.2 Open file in write mode
        fp = fopen("registrations.txt", "a+");

        if (fp == NULL)
        {
            perror("Error opening file");
            printf("ERROR: (Server) Failed to open file in write mode\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("INFO: (Server) File opened in write mode\n");
        }

        for (i = 0; i < num_students; i++)
        {
            fprintf(fp, "%d\t\t\t\t%s\t\t\t%s\t%s\n", students[i].serial_number, students[i].reg_number, students[i].first_name, students[i].last_name);
        }

        fclose(fp);

        // 9. Formulate a response message based on the result of the data processing - SUCCESS
        char success_message[] = "INFO: (Server) Student details saved successfully\n";
        send(client_socket, success_message, strlen(success_message), 0);
    }
    else
    {
        // 9. Formulate a response message based on the result of the data processing - FAILURE
        char failure_message[] = "ERROR: (Server) Student already exists; Registration number or serial number already exists\n";
        send(client_socket, failure_message, strlen(failure_message), 0);
    }

    // 10. Close the child process socket and exit
    close(client_socket);

    printf("INFO: (Server) Client socket closed\n");
    printf("INFO: (Server) Client handler exiting\n");

    


}

// Main function
int main()
{
    // 1. Create a socket with the socket() system call
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        printf("ERROR: (Server) Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket created\n");
    }

    // Create a server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    server_addr.sin_port = htons(SERVER_PORT);

    // 2. Bind the socket to an address using the bind() system call
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket to address");
        printf("ERROR: (Server) Failed to bind socket to address\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Socket bound to address\n");
    }

    // 3. Listen for connections with the listen() system call. Have a maximum number of connections that can be queued - 5
    if (listen(server_socket, 5) < 0)
    {
        perror("Error listening on socket");
        printf("ERROR: (Server) Failed to listen on socket\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("INFO: (Server) Listening on socket\n");
    }

    printf("Server started. Listening on port %d\n", SERVER_PORT);

    // 4. Continually accept incoming connections with clients using the accept() system call
    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            perror("Error accepting connection");
            printf("ERROR: (Server) Failed to accept connection\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("INFO: (Server) Accepted connection\n");
        }

        // 5. Fork a child process to handle each client connection
        pid_t pid = fork();

        if (pid < 0) // Server failed to fork child process
        {
            perror("Error forking child process");
            printf("ERROR: (Server) Failed to fork child process\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) // Child process process id
        {
            // Call the client handler function
            client_handler((void *)&client_socket);
        }
        else // Parent process process id
        {
            close(client_socket);
        }
    }
}
