#include <stdbool.h>

#define MESSAGE_SIZE 1024
#define PORT 8001
#define IP_ADDRESS "127.0.0.1"


struct Stock {
	short currency;
	char name[10];
	bool trend;
	short daysOfTrend;
};

struct Person {
	short* quantityStocks;
	int money;
};


enum Command {
	HELP,
	BUY,
	SELL,
	SKIP,
	EXIT,
	STAT
};

static const char* commands[] = {"help", "buy", "sell", "skip", "exit", "stat"};
