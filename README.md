# Sockets

An implementation of client-server programs with different implementations of the server algorithms in C
The steps followed in each of the implementations is found in this chat: [chat](https://sharegpt.com/c/I2XyIl1)

## Iterative Connectionless Server

### Server Side steps

1. Create a socket for the server using `socket()` function.
2. Bind the socket to a specific port using the `bind()` function.
3. Receive data from the client using the `recvfrom()` function.
4. Parse the data sent by the client and check if the registration number and serial number are unique.
5. If the registration number and serial number are unique, save the data in a text file with the name registrations.txt in the specified format.
6. Send a response to the client confirming that the registration was successful using the `sendto()` function.
7. Go back to step 3 and repeat the process for the next client.

### Client Side steps

1. Create a socket for the client using the `socket()` function.
2. Prompt the user to enter their details including the unique student registration number, a unique serial number, and first and last name.
3. Send the data to the server using the `sendto()` function.
4. Wait for a response from the server using the `recvfrom()` function.
5. Print the response received from the server to confirm the registration was successful.
6. Close the connection using the `close()` function.

## Iterative Connection-Oriented Server

### Server Side steps

1. Create a socket for the server using the `socket()` function.
2. Bind the socket to a specific port using the `bind()` function.
3. Listen for incoming connections using the `listen()` function.
4. Accept incoming connections using the `accept()` function.
5. While the server is running, read the data sent by the client using the `recv()` function.
6. Parse the data sent by the client and check if the registration number and serial number are unique.
7. If the registration number and serial number are unique, save the data in a text file with the name registrations.txt in the specified format.
8. Send a response to the client confirming that the registration was successful using the `send()` function.
9. Close the connection using the `close()` function.
10. Go back to step 4 and repeat the process for the next client.

### Client Side Steps

1. Create a socket for the client using the `socket()` function.
2. Connect to the server using the `connect()` function.
3. While the client is running, prompt the user to enter their details including the unique student registration number, a unique serial number, and first and last name.
4. Send the data to the server using the `send()` function.
5. Wait for a response from the server using the `recv()` function.
6. Print the response received from the server to confirm the registration was successful.
7. Go back to step 3 and repeat the process for the next registration, or close the client socket using the `close()` function to terminate the client.

## Concurrent Connectionless Server

### Server Side Steps

1. Create a socket for the server using the `socket()` function.
2. Bind the socket to a specific port using the `bind()` function.
3. While the server is running, receive incoming packets from the clients using the `recvfrom()` function in a loop.
4. Create a new thread to handle each received packet.
5. In each thread, parse the data sent by the client and check if the registration number and serial number are unique.
6. If the registration number and serial number are unique, save the data in a text file with the name registrations.txt in the specified format.
7. Send a response to the client confirming that the registration was successful using the `sendto()` function.
8. Go back to step 3 and repeat the process for the next packet.

### Client Side Steps

1. Create a socket for the client using the `socket()` function.
2. Set the destination address and port using the `sendto()` function.
3. While the client is running, prompt the user to enter their details including the unique student registration number, a unique serial number, and first and last name.
4. Send the data to the server using the `sendto()` function.
5. Wait for a response from the server using the `recvfrom()` function.
6. Print the response received from the server to confirm the registration was successful.
7. Go back to step 3 and repeat the process for the next registration, or close the client socket using the `close()` function to terminate the client.

## Concurrent Connection-Oriented Server

### Server Side Steps

1. Create a socket for the server using the `socket()` function.
2. Bind the socket to a specific port using the `bind()` function.
3. Listen for incoming connections using the `listen()` function.
4. While the server is running, accept incoming connections using the `accept()` function and create a new thread to handle each connection.
5. In each thread, read the data sent by the client using the `recv()` function.
6. Parse the data sent by the client and check if the registration number and serial number are unique.
7. If the registration number and serial number are unique, save the data in a text file with the name registrations.txt in the specified format.
8. Send a response to the client confirming that the registration was successful using the `send()` function.
9. Close the connection using the `close()` function.
10. Go back to step 5 and repeat the process for the next client.

### Client Side Steps

1. Create a socket for the client using the `socket()` function.
2. Connect to the server using the `connect()` function.
3. While the client is running, prompt the user to enter their details including the unique student registration number, a unique serial number, and first and last name.
4. Send the data to the server using the `send()` function.
5. Wait for a response from the server using the `recv()` function.
6. Print the response received from the server to confirm the registration was successful.
7. Go back to step 3 and repeat the process for the next registration, or close the client socket using the `close()` function to terminate the client.
