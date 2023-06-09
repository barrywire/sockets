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

    // 1. Create client socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation error");
        printf("ERROR: (Client)  Socket creation error \n");
        return -1;
    }
    else
    {
        printf("INFO: (Client) Socket created - %d\n", sock);
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
        // 2. Prompt user for their student data
        printf("Enter the student's details:\n");
        printf("Serial Number: ");
        scanf("%d", &student.serialNumber);
        printf("Registration Number: ");
        scanf("%s", student.regNumber);
        printf("First Name: ");
        scanf("%s", student.firstName);
        printf("Last Name: ");
        scanf("%s", student.lastName);

        // 3. Send the student's details to the server
        sprintf(buffer, "%d %s %s %s", student.serialNumber, student.regNumber, student.firstName, student.lastName);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (errno)
        {
            perror("Error sending data");
            printf("ERROR: (Client) Failed to send data\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("INFO: (Client) Data sent to server successfully\n");
        }
        // printf("Student data sent successfully.\n");

        // 4. Receive the response from the server
        int serv_addr_len = sizeof(serv_addr);
        valread = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&serv_addr, &serv_addr_len);
        printf("%s\n", buffer);
        printf("\n");
    }
    close(sock);

    return 0;
}
