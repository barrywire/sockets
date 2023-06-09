#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8080

struct Student
{
    /* Student Properties */
    int serialNumber;
    char regNumber[20];
    char firstName[50];
    char lastName[50];
};

int main()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    struct Student student;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    while (1)
    {
        // Get the student's details from the user
        printf("Enter the student's details:\n");
        printf("Serial Number: ");
        scanf("%d", &student.serialNumber);
        printf("Registration Number: ");
        scanf("%s", student.regNumber);
        printf("First Name: ");
        scanf("%s", student.firstName);
        printf("Last Name: ");
        scanf("%s", student.lastName);

        // Send the student's details to the server
        sprintf(buffer, "%d %s %s %s", student.serialNumber, student.regNumber, student.firstName, student.lastName);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("Student data sent successfully.\n");

        // Receive the response from the server
        int serv_addr_len = sizeof(serv_addr);
        valread = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&serv_addr, &serv_addr_len);
        printf("%s\n", buffer);
        printf("\n");
    }
    close(sock);

    return 0;
}
