#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    // Create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Read student data from user
    struct student new_student;
    printf("Enter serial number: ");
    scanf("%d", &new_student.serial_number);
    printf("Enter registration number: ");
    scanf("%s", new_student.reg_number);
    printf("Enter first name: ");
    scanf("%s", new_student.first_name);
    printf("Enter last name: ");
    scanf("%s", new_student.last_name);

    // Send student data to server
    sendto(client_socket, (void *)&new_student, sizeof(new_student), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive response from server
    char response[100];
    socklen_t server_addr_len = sizeof(server_addr);
    recvfrom(client_socket, response, sizeof(response), 0, (struct sockaddr *)&server_addr, &server_addr_len);
    printf("%s\n", response);

    // Close socket
    close(client_socket);

    return 0;
}
