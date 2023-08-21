#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include"common.h"


int main() {
	int socket_desc;
	struct sockaddr_in server_addr;
	char server_msg[MESSAGE_SIZE] = "a", client_msg[MESSAGE_SIZE] = "Abobus";
	
	memset(server_msg, '\0', sizeof(server_msg));

	printf("Connected to server!\n");

	while (true) {
	memset(server_msg, '\0', sizeof(server_msg));
	memset(client_msg, '\0', sizeof(client_msg));
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Socket hasn't been built!\n");
		return -1;
	}
	printf("Socket is created\n");
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			printf("Unable to connect\n");
			return -1;
	}
		fgets(client_msg, sizeof(client_msg), stdin);
	
		if (send(socket_desc, client_msg, strlen(client_msg), 0) < 0) {
			printf("Error while sending message\n");
			return -1;
		}
		printf("Sent");
		
		printf("Answer from client: %s\n", server_msg);
		int recvn = recv(socket_desc, server_msg, sizeof(server_msg), 0);
		printf("%d", recvn);
		if (recvn < 0) {
			printf("Error while recieving message\n");
			return -1;
		}
		printf("Answer from server: %s\n", server_msg);
	}
	
	
	close(socket_desc);
	return 0;
}
