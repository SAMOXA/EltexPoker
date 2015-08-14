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
};

////////////////////From game server logic structures/////////
enum gameStates {
	GAME_START,
	GAME_PRE_FLOP,
	GAME_FIRST_ROUND,
	GAME_FIRST_RISE,
	GAME_FLOP,
	GAME_ROUND,
	GAME_ROUND_RISE,
	GAME_TERN,
	GAME_RIVER,
	GAME_FINAL
};

enum playerStates {
	PLAYER_ACTIVE,
	PALYER_PASSIVE,
	PLAYER_PASS,
	PLAYER_CONNECTING,
	PLAYER_WAIT,
	PLAYER_DISCONNECTED,
	PLAYER_FREE
};

enum serverMessagesTypes {
	STATE_NEW_PLAYER,
	STATE_PLAYER_STATE_CHANGED,
	STATE_FULL_UPDATE,
	STATE_ACTIVE_PLAYER_CHANGED,
	STATE_NEW_CARD_TABLE,
	STATE_NEW_CARD_PALAYER
};

enum clientMessagesTypes {
	ACTION_EXIT,
	ACTION_CALL,
	ACTION_RISE,
	ACTION_FOLD,
	ACTION_CHECK,
	ACTION_ALL_IN
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
