#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONNECTIONS 5

typedef struct student
{
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
} student;

student students[MAX_CONNECTIONS];
int num_students = 0;

pthread_mutex_t mutex;

void add_student(char *reg_number, char *first_name, char *last_name)
{
    pthread_mutex_lock(&mutex);
    num_students++;
    students[num_students - 1].serial_number = num_students;
    strcpy(students[num_students - 1].reg_number, reg_number);
    strcpy(students[num_students - 1].first_name, first_name);
    strcpy(students[num_students - 1].last_name, last_name);
    pthread_mutex_unlock(&mutex);
}

int check_unique(char *reg_number)
{
    for (int i = 0; i < num_students; i++)
    {
        if (strcmp(students[i].reg_number, reg_number) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void save_students()
{
    FILE *fp = fopen("registrations.txt", "w");
    fprintf(fp, "Serial Number\tRegistration Number\tNames\n");
    for (int i = 0; i < num_students; i++)
    {
        fprintf(fp, "%d\t%s\t%s %s\n", students[i].serial_number, students[i].reg_number, students[i].first_name, students[i].last_name);
    }
    fclose(fp);
}

void *handle_connection(void *socket_ptr)
{
    int socket = *((int *)socket_ptr);
    char buffer[1024] = {0};
    char response[1024] = {0};
    int valread;

    valread = recv(socket, buffer, 1024, 0);
    buffer[valread] = '\0';
    printf("Received data: %s\n", buffer);

    char *reg_number = strtok(buffer, ",");
    char *first_name = strtok(NULL, ",");
    char *last_name = strtok(NULL, ",");

    int unique_reg = check_unique(reg_number);
    if (unique_reg)
    {
        add_student(reg_number, first_name, last_name);
        save_students();
        strcpy(response, "Registration successful.");
    }
    else
    {
        strcpy(response, "Registration failed. Registration number already exists.");
    }

    send(socket, response, strlen(response), 0);
    close(socket);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind server socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections and create new thread to handle each connection
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, (void *)&new_socket) < 0)
        {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
