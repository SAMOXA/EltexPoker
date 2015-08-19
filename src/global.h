#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LENGTH 10
#define MAX_PASS_LENGTH 16
#define MAX_PLAYERS_PER_TABLE 4
#define MAX_TABLES_COUNT 5
#define SESSION_TOKEN_LENGTH 16
#define FALSE_CARD 60

enum lears{
	HEARTS,
	DIAMONDS,
	CLUBS,
	SPADES
};

enum values{
	TWO = 0,
	THREE = 1,
	FOUR = 2,
	FIVE = 3,
	SIX = 4,
	SEVEN = 5,
	EIGHT = 6,
	NINE = 7,
	TEN = 8,
	JACK = 9,
	QUEEN = 10,
	KING = 11,
	ACE = 12
};

#define GET_CARD_LEAR(x) (x%4)
#define GET_CARD_VALUE(x) ((unsigned char)x/4)
#define CREATE_CARD(value, lear) ((value*4)+lear)

struct loginRequest_t {
	char name[MAX_NAME_LENGTH];
	char pass[MAX_PASS_LENGTH];
	char registerFlag;
};

struct table_t {
	char id;
	unsigned char tables[MAX_PLAYERS_PER_TABLE][MAX_NAME_LENGTH];
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
	GAME_START, //Только на старте стола или если игрок остался один
	GAME_PRE_FLOP_ROUND,
	GAME_FLOP_ROUND,
	GAME_TERN_ROUND,
	GAME_RIVER_ROUND,
	GAME_FINAL,
	GAME_INSTANT_WIN //Если все пасанули
};

enum playerStates {
	PLAYER_FREE,
	PLAYER_ACTIVE,
	PLAYER_PASS,
	PLAYER_CONNECTING,
	PLAYER_DISCONNECTED
};

enum serverMessagesTypes {
	STATE_NEW_PLAYER,
	STATE_PLAYER_STATE_CHANGED,
	STATE_FULL_UPDATE,
	STATE_ACTIVE_PLAYER_CHANGED,
	STATE_NEW_CARD_TABLE,
	STATE_NEW_CARD_PALAYER,
	STATE_PLAYER_DISCONECTED,
	STATE_ERROR
};

enum clientMessagesTypes {
	ACTION_EXIT,
	ACTION_CALL,
	ACTION_RISE,
	ACTION_FOLD,
	ACTION_CHECK,
	ACTION_ALL_IN,
	ACTION_CONNECT_REQUEST
};

struct player_t {
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	unsigned int state;
	unsigned int bet;
	unsigned int money;
	unsigned char cards[2];
	unsigned int session;
};

struct gameState_t {
	unsigned int id;
	unsigned char state;
	unsigned char activePlayerId;
	unsigned int bet;
	char lastRisePlayerId;
	unsigned int lastRiseCount;
	unsigned int bank;
	unsigned int dillerId;
	unsigned int smallBlindPlayerId;
	unsigned int bigBlindPlayerId;
	unsigned char cards[5];
	struct player_t players[MAX_PLAYERS_PER_TABLE];
	unsigned char playersCount;
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

enum errorTypes_e {
	ERROR_PERMISSION,
	ERROR_AUTH,
	ERROR_RISE,
	ERROR_MONEY
};

struct errorMsg_t {
	int type;
	char msg[100];
};
//////////////////////////////////////////////////////
#endif //GLOBAL_H
