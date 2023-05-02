#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

struct student {
    int serial_number;
    char reg_number[20];
    char first_name[50];
    char last_name[50];
};

int main(int argc, char *argv[]) {
    int client_socket, recv_size;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    struct student student_info;

    // Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        printf("Could not create socket\n");
        return 1;
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(SERVER_PORT);

    // Prompt user for student information
    printf("Enter student information:\n");
    printf("Registration Number: ");
    scanf("%s", student_info.reg_number);
    printf("Serial Number: ");
    scanf("%d", &student_info.serial_number);
    printf("First Name: ");
    scanf("%s", student_info.first_name);
    printf("Last Name: ");
    scanf("%s", student_info.last_name);

    // Send data to server
    sprintf(buffer, "%d,%s,%s,%s", student_info.serial_number, student_info.reg_number, student_info.first_name, student_info.last_name);
    if (sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Send failed");
        return 1;
    }

    // Receive response from server
    memset(buffer, 0, BUFFER_SIZE);
    if ((recv_size = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL)) < 0) {
        perror("Receive failed");
        return 1;
    }

    // Print response from server
    printf("Server response: %s\n", buffer);

    // Close connection
    close(client_socket);

    return 0;
}
