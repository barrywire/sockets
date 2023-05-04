#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char registration_number[20];
    char first_name[50];
    char last_name[50];
    int serial_number = 0;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Prompt user for registration details
    while (1)
    {
        printf("Enter registration number (e.g. P15/1254/2020): ");
        scanf("%s", registration_number);

        printf("Enter serial number: ");
        scanf("%d", &serial_number);

        printf("Enter first name: ");
        scanf("%s", first_name);

        printf("Enter last name: ");
        scanf("%s", last_name);

        // Create registration data string
        char registration_data[1024] = {0};
        snprintf(registration_data, 1024, "%s,%d,%s,%s", registration_number, serial_number, first_name, last_name);

        // Send registration data to server
        send(sock, registration_data, strlen(registration_data), 0);

        // Wait for response from server
        valread = read(sock, buffer, 1024);

        printf("%s\n", buffer);

        // Prompt user to continue or exit
        char choice;
        printf("Do you want to register another student? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'n' || choice == 'N')
        {
            break;
        }
    }

    close(sock);
    return 0;
}
