#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int respond_client(int connfd) {
	char read_buffer[32] = {};
	if (read(connfd, read_buffer, sizeof(read_buffer) - 1) < 0) {
		printf("read() error %s... \n", strerror(errno));
		return -1;
	}char write_buffer[] = "+PONG\r\n";
	write(connfd, write_buffer, strlen(write_buffer));

	return 0;
}

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	//
	int server_fd, client_addr_len;
	 struct sockaddr_in client_addr;
	
	 server_fd = socket(AF_INET, SOCK_STREAM, 0);
	 if (server_fd == -1) {
	 	printf("Socket creation failed: %s...\n", strerror(errno));
	 	return 1;
	 }
	
	// // Since the tester restarts your program quite often, setting REUSE_PORT
	// // ensures that we don't run into 'Address already in use' errors
	 int reuse = 1;
	 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
	 	printf("SO_REUSEPORT failed: %s \n", strerror(errno));
	 	return 1;
	 }
	
	 struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
	 								 .sin_port = htons(6379),
	 								 .sin_addr = { htonl(INADDR_ANY) },
	 								};
	
	 if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
	 	printf("Bind failed: %s \n", strerror(errno));
	 	return 1;
	 }
	
	 int connection_backlog = 5;
	 if (listen(server_fd, connection_backlog) != 0) {
	 	printf("Listen failed: %s \n", strerror(errno));
	 	return 1;
	 }
	
	 printf("Waiting for a client to connect...\n");
	 client_addr_len = sizeof(client_addr);
	 int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	 
	 printf("Client connected\n");

	 char buffer[1024];
	 memset(buffer, 0, sizeof(buffer));

	 while (1) {
        // Receive data from the client
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        if (bytes_received <= 0) {
            // Handle error or client disconnection
            break;
        }

        // Check if the received data is a PING command
        if (strncmp(buffer, "PING\r\n", 6) == 0) {
            // Respond with "+PONG\r\n"
            const char* response = "+PONG\r\n";
            send(client_socket, response, strlen(response), 0);
        }

        // Clear the buffer for the next iteration
        memset(buffer, 0, sizeof(buffer));
    }

	 if (respond_client(client_fd) < 0) {
		printf("Unable to respond to client properly \n");
	 }
	close(client_socket);
	close(server_fd);

	return 0;
}
