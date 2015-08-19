#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LENGTH 10
#define MAX_PASS_LENGTH 16
#define MAX_PLAYERS_PER_TABLE 4
#define MAX_TABLES_COUNT 5
#define SESSION_TOKEN_LENGTH 16
#define MAX_ERROR_MSG_LEN 24
#define STATUS_OK 1
#define STATUS_BAD 0

#define FALSE_CARD 60

enum lears {
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

};

struct table_t {
	int id;
	char tables[MAX_PLAYERS_PER_TABLE][MAX_NAME_LENGTH];
};

struct loginResponce_t {
	int status;
	char errorBuf[MAX_ERROR_MSG_LEN];
};

struct selectRequest_t {
	char name[MAX_NAME_LENGTH];
	int tableID;
};

// struct selectRequest_t {
// 	char token; //MD5(pass+login+counter)
// 	char selectedId;
// };

struct selectResponce_t {
	int status;
	int port;
	int session;
	char error[MAX_ERROR_MSG_LEN];
};


////////////////////From lobbi server logic structures////////
enum initAction {
	REGISTRATION,
	LOG_IN,
	CREATE_TABLE,
	CONNECT_TO_TABLE,
	LIST_TABLE
};
//////////////////////////////////////////////////////////////


////////////////////From game server logic structures/////////
enum gameStates {
	GAME_START, //Только на старте стола или если игрок остался один
	GAME_PRE_FLOP,
	GAME_FIRST_ROUND,
	GAME_FLOP_ROUND,
	GAME_TERN_ROUND,
	GAME_RIVER_ROUND,
	GAME_FINAL //Сразу переходит в префлоп
};

enum playerStates {
	PLAYER_ACTIVE,
	PLAYER_PASSIVE,
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
	unsigned char betPlayerId;
	unsigned int bet;
	unsigned char wasRise;
	unsigned char lastRisePlayerId;
	unsigned int lastRiseCount;
	unsigned int bank;
	unsigned int dillerId;
	unsigned char cards[5];
	struct player_t players[MAX_PLAYERS_PER_TABLE];
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
