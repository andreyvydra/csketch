#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include <limits.h>
#include "common.h"


void updateTrends();
void updateCurrencies();
void printMarket();
void printId(int id);
void printStockName(int id);
void printCurrency(int id);
void printColumn();
void sendHelp();
void sendStatistic();
void init();
void decrementTrendsDays();
void freeResources();
void updateMarket();
void loadData();
void writeData();
void sendMessage(char* message);

short getNumberLen(int number);
short getCommandNumber(char* command);

int startServer();

short nameLenght = 10;
short daysToAdd = 5;
short marketSize = 5;
bool running = true;

struct Stock *stocks;
struct Person person;

int socket_desc, client_socket_desc, client_size;
struct sockaddr_in server_addr, client_addr;
char server_msg[MESSAGE_SIZE] = "Abobus", client_msg[MESSAGE_SIZE];


int main() {
	init();
	printf("%d\n", *(person.quantityStocks));
	char command[10];
	short stockId;
	short quantityToDo;
	while (running) {
		printMarket();
		memset(server_msg, '\0', sizeof(server_msg));
	memset(client_msg, '\0', sizeof(client_msg));
		if (listen(socket_desc, 8) < 0) {
				printf("Can't listen\n");
				return -1;
		}
		client_size = sizeof(client_socket_desc);
		client_socket_desc = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
		printf("Listening for message client\n");
		
		if (client_socket_desc < 0) {
			printf("Error while accepting!\n");
			return -1;
		}
		
		if (recv(client_socket_desc, client_msg, sizeof(client_msg), 0) < 0) {
			printf("Command hasn't been recieved\n");
			return -1;
		}
		printf("Command from client: %s\n", client_msg);
		
		sscanf(client_msg, "%s%d%d", command, &stockId, &quantityToDo);

		short commandNumber = getCommandNumber(command);
		switch (commandNumber) {
			case (HELP):
				sendHelp();
				continue;
			case (BUY):
				
				if (person.money - quantityToDo * (*(stocks + stockId - 1)).currency < 0) {
					printf("Invalid operation. Not enough money!\n");
				} else {
					person.money -= quantityToDo * (*(stocks + stockId - 1)).currency;
					*(person.quantityStocks + stockId - 1) += quantityToDo;
				}
				printf("%hd\n", commandNumber);
				sendMessage("Buy");
				break;
			case (SKIP):
				sendMessage("Aboba");
				break;
			case (SELL):
				sscanf(client_msg, "%s %d %d", &command, &stockId, &quantityToDo);
				if (*(person.quantityStocks + stockId - 1) < quantityToDo) {
					printf("Invalid operation. Not enough stocks!\n");
				} else {
					person.money += quantityToDo * (*(stocks + stockId - 1)).currency;
					*(person.quantityStocks + stockId - 1) -= quantityToDo;
				}
				sendMessage("Sell");
				break;
			case (STAT):
				sendStatistic();
				break;
			case (EXIT):
				printf("Goodbye!\n");
				running = false;
				continue;
			default:
				printf("Invalid operation!\n");
				sendMessage("Invalid");
				continue;
		}
		daysToAdd--;
		decrementTrendsDays();
		updateTrends();
		updateCurrencies();
		updateMarket();
		writeData();		

	}
	freeResources();

	return 0;
}

void init() {
	// Init random seed
	assert (startServer() != -1);
	srand(time(NULL));
	loadData();
	strcpy(stocks[0].name, "Amazon");
	strcpy(stocks[1].name, "Ozon");
	strcpy(stocks[2].name, "Bakery");
	
}


int startServer() {
	
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Socket hasn't been built!\n");
		return -1;
	}
	printf("Socket is created\n");
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	
	if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Port is not available!\n");
		return -1;
	}
	printf("Succesful binding");

	return 0;
}

void sendMessage(char* message) {
	if (send(client_socket_desc, message, strlen(message), 0) < 0) {
		printf("Send.... Error\n");
	}
}


void loadData() {
printf("1");
	FILE* filePtr = fopen("2.txt", "r");
	
	// Dyanmic arrays init before reading file
	person.quantityStocks = (short*) malloc(marketSize * sizeof(short));
	printf("%d\n", *(person.quantityStocks));
	stocks = (struct Stock*) malloc(marketSize * sizeof(struct Stock));
	
	if(filePtr != NULL) {
		fread(&marketSize, sizeof(short), 1, filePtr);
		fread(&person.money, sizeof(int), 1, filePtr);
		fread(person.quantityStocks, sizeof(short), marketSize, filePtr);
		fread(stocks, sizeof(struct Stock), marketSize, filePtr);
		fclose(filePtr);
	} else {
		person.money = 30000;
		updateTrends();
	}
}

void writeData() {
	FILE* filePtr = fopen("2.txt", "w");
	
	if(filePtr != NULL) {
		fwrite(&marketSize, sizeof(short), 1, filePtr);
		fwrite(&person.money, sizeof(int), 1, filePtr);
		fwrite(person.quantityStocks, sizeof(short), marketSize, filePtr);
		fwrite(stocks, sizeof(struct Stock), marketSize, filePtr);
	}
	fclose(filePtr);
}


void updateMarket() {
	if (daysToAdd == 0) {
		daysToAdd = 5;
		if (marketSize > 100) {
			return;
		}
		marketSize++;
		stocks = (struct Stock*) realloc(stocks, marketSize * sizeof(struct Stock));
		person.quantityStocks = (short) realloc(person.quantityStocks, marketSize * sizeof(short));;
		char name[10];
		for (int i = 0; i < 4; i++) {
			(*(stocks + marketSize - 1)).name[i] = rand() % 27 + 65;
		}
	}
}

void freeResources() {
	close(client_socket_desc);
	close(socket_desc);
	free(stocks);
}


void decrementTrendsDays() {
	for (int i = 0; i < marketSize; i++) {
		--(*(stocks + i)).daysOfTrend;
	
	}
}


void updateTrends() {
	for (int i = 0; i < marketSize; i++) {
		if ((*(stocks + i)).daysOfTrend == 0) {
			(*(stocks + i)).daysOfTrend = (int) (rand() % 10 + 1);
			(*(stocks + i)).trend = (bool) (rand() % 2);
		}
	}
}

void updateCurrencies() {
	for (int i = 0; i < marketSize; i++) {
		if ((*(stocks + i)).trend && (*(stocks + i)).currency <= SHRT_MAX) {
			(*(stocks + i)).currency += rand() % 100;
		} else if (!(*(stocks + i)).trend && (*(stocks + i)).currency >= SHRT_MIN) {
			(*(stocks + i)).currency -= rand() % 100;
			printf("\n %b \n", !(*(stocks + i)).trend);
		}
	}
}

void printMarket() {
	printf("+--------------------------------+\n");
	printf("|  #  |    Name    |    Price    |\n");
	printf("+--------------------------------+\n");
	for (int i = 0; i < marketSize; i++) {
		printId(i);
		printStockName(i);
		printCurrency(i);
		printf("\n");
	}
	printf("+--------------------------------+\n");
}

void printId(int id) {
	// Calculate spaces end number len to print it correctly
	// i + 1 for human vision
	short lenNumber = getNumberLen(id + 1);
	printf("|");
	double space = (5 - lenNumber) / 2.0;
	short leftSpace = floor(space);
	short rightSpace = ceil(space);
	for (short j = 0; j < leftSpace; j++) {
		printf(" ");
	}
	printf("%d", id + 1);
	for (short j = 0; j < rightSpace; j++) {
		printf(" ");
	}
	printf("|");
}

void printStockName(int id) {
	char* name = (*(stocks + id)).name;
	short lenName = strlen(name);
	printf("%h", lenName);
	for (int i = 0; i < floor((12 - lenName) / 2.0); i++) {
		printf(" ");
	}
	printf("%s", name);
	for (int i = 0; i < ceil((12-lenName) / 2.0); i++) {
		printf(" ");
	}
	printf("|");
}

void printCurrency(int id) {
	short currency = (*(stocks + id)).currency;
	short lenNumber = getNumberLen(currency);
	double space = (13 - lenNumber) / 2.0;
	short leftSpace = floor(space);
	short rightSpace = ceil(space);
	for (short j = 0; j < leftSpace; j++) {
		printf(" ");
	}
	printf("%d", currency);
	for (short j = 0; j < rightSpace; j++) {
		printf(" ");
	}
	printf("|");
}

void printColumn() {
}

short getNumberLen(int number) {
	short count = 0;
	if (number < 0) {
		count++;
	}
	while (number != 0) {
		count += 1;
		number /= 10;
	}
	return count;
}

void sendHelp() {
	sendMessage("HELP!\nbuy what_to_buy quantity\nsell what_to_sell quantity\nskip\n");
}

void sendStatistic() {
	snprintf(server_msg, sizeof(server_msg), "Money - %d", person.money);
	sendMessage(server_msg);
}

short getCommandNumber(char* command) {
	for (short i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
		if (strcmp(command, commands[i]) == 0) {
			return i; 
		}
	}
	return -1;
}
