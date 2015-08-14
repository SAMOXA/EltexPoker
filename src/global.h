#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LENGTH 10
#define MAX_PASS_LENGTH 16
#define MAX_PLAYERS_PER_TABLE 4
#define MAX_TABLES_COUNT 5
#define SESSION_TOKEN_LENGTH 16

enum lears{
	HEARTS,
	DIAMONDS,
	CLUBS,
	SPADES
};

#define GET_CARD_LEAR(x) (x%4)
#define GET_CARD_VALUE(x) ((unsigned char)x/4)

struct loginRequest_t {
	char name[MAX_NAME_LENGTH];
	char pass[MAX_PASS_LENGTH];
	char registerFlag;
};

struct table_t {
	char id;
	unsigned char [MAX_PLAYERS_PER_TABLE][MAX_NAME_LENGTH];
};

struct loginResponce_t {
	char status;
	int startCounter;
	struct table_t tables[MAX_TABLES_COUNT];
};

struct selectRequest_t {
	char token; //MD5(pass+login+counter)
	char selectedId;
};

struct selectResponce_t {
	int port;
}

////////////////////From game server logic structures/////////
enum gameStates {
	START,
	PRE_FLOP,
	FIRST_ROUND,
	FIRST_RISE,
	FLOP,
	ROUND,
	ROUND_RISE,
	TERN,
	RIVER,
	FINAL
};

enum playerStates {
	ACTIVE,
	PASSIVE,
	PASS,
	CONNECTING,
	WAIT,
	DISCONNECTED
};

enum serverMessagesTypes {
	NEW_PLAYER,
	PLAYER_STATE_CHANGED,
	FULL_UPDATE,
	ACTIVE_PLAYER_CHANGED,
	NEW_CARD_TABLE,
	NEW_CARD_PALYE
};

enum clientMessagesTypes {
	EXIT,
	CALL,
	RISE,
	FOLD,
	CHECK,
	ALL_IN
};

struct player_t {
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	unsigned int state;
	unsigned int bet;
	unsigned int money;
	unsigned char cards[2];
};

struct gameState_t {
	unsigned int id;
	unsigned char state;
	unsigned char betPlayerIndex;
	unsigned int bet;
	unsigned char lastRisePlayerIndex;
	unsigned int lastRiseCount;
	unsigned int bank;
	unsigned char cards[5];
	player_t players[MAX_PLAYERS_PER_TABLE];
};

struct changeActivePlayer_t {
	unsigned int id;
	unsigned int lastBet;
	unsigned int bank;
};

struct changeCardOnTable_t {
	unsigned char cards[5];
};

struct changePlayerCards_t {
	unsigned char cards[2];
};

struct changePlayerState_t {
	unsigned char state;
};
//////////////////////////////////////////////////////
#endif //GLOBAL_H
