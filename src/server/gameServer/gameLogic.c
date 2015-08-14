#include "gameLogic.h"

enum permissionsTypes_e {
	PERM_CHECK,
	PERM_RISE,
	PERM_CALL,
	PERM_ALL_IN,
	PERM_FOLD
};

struct gameState_t game;

int getPlayerIndex(int id) {
	int i;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].id == id){
			return i;
		}
	}
	return -1;
}

char playerIsActive(int id) {
	int index = getPlayerIndex(id);
	return game.players[index].state == PLAYER_ACTIVE;
}

//TODO pending buffer
void addNewPlayer(struct newPlayer_t *newPlayer) {
	int i;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].state == PLAYER_FREE){
			break;
		}
	}
	game.players[i].state = PLAYER_CONNECTING;
	game.players[i].id = newPlayer->id;
	game.players[i].money = newPlayer->money;
	game.players[i].session = newPlayer->session;
	memcpy(game.players[i].name, newPlayer->name, sizeof(char) * MAX_NAME_LENGTH);
}


void removePlayer(int id){
	int index = getPlayerIndex(id);
	memset(&game.players[index], 0, sizeof(struct player_t));
	game.players[index].state = PLAYER_FREE;
}

char getNextPlayer() {
	int curIndex = game.betPlayerId;
	int newIndex = game.betPlayerId;
	while(1){
		newIndex = (newIndex + 1) % MAX_PLAYERS_PER_TABLE;
		if(playerIsActive(game.players[newIndex].id)){
			return newIndex;
		}
	}
}

char checkPlayerPermission(int id, char type, int riseCount){
	int index = getPlayerIndex(id);
	if(game.state == GAME_START || game.state == GAME_FINAL){
		return 0;
	}
	if(game.betPlayerId != id){
		return 0;
	}
	switch(type){
		case(PERM_CHECK):
			if(game.lastRisePlayerId != id &&
					game.wasRise != 0 || game.state == GAME_PRE_FLOP)
			{
				return 0;
			}
			break;
		case(PERM_FOLD):
		case(PERM_ALL_IN):
			break;
		case(PERM_RISE):
			if(game.players[index].money - (game.bet + riseCount) < 0){
				return 0;
			}
			break;
		case(PERM_CALL):
			if(game.players[index].money - game.bet < 0){
				return 0;
			}
			break;
	}
	return 1;
}

char checkWin() {
	char activePlayersCount = 0;
	int i;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++) {
		if(playerIsActive(game.players[i].id)){
			activePlayersCount++;
		}
	}
	return activePlayersCount == 1;
}

char checkNextRound(int id) {
	if(id == game.lastRisePlayerId){
		return 1;
	}
	return 0;
}

void goNextRound() {
	int winIndex = -1;
	int i;
	game.state++;
	if(game.state == GAME_FINAL){
		//winIndex = getWinIndex()
		game.players[winIndex].money += game.bank;
		game.bank = 0;
		game.dillerId = (game.dillerId + 1) % MAX_PLAYERS_PER_TABLE;
		game.bet = 2;
		game.wasRise = 0;
		game.lastRisePlayerId = (game.dillerId + 2) % MAX_PLAYERS_PER_TABLE;
		game.lastRiseCount = 0;
		//reset cards
		memset(game.cards, FALSE_CARD, sizeof(unsigned char)*5);
		for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
			memset(game.players[i].cards, FALSE_CARD, sizeof(unsigned char)*2);
		}
		//Blinds
		game.players[(game.dillerId + 1) % MAX_PLAYERS_PER_TABLE].money -= game.bet/2;
		game.players[(game.dillerId) % MAX_PLAYERS_PER_TABLE].money -= game.bet;
		game.bank += game.bet/2 + game.bet;

		game.state = GAME_PRE_FLOP;
	}else{
		game.wasRise = 0;
		game.lastRiseCount = 0;
		game.lastRisePlayerId = (game.dillerId + 1) % MAX_PLAYERS_PER_TABLE;
		//cards
	}
}

int checkStateChange(int id) {
	if(checkWin() || checkNextRound(id)){
		if(checkWin()){
			game.state = GAME_RIVER_ROUND;
		}
		goNextRound();
		//send new state
		return 1;
	}
	return 0;
}

char actionCall(int id){
	int index = getPlayerIndex(id);
	if(checkPlayerPermission(id, PERM_CALL, 0)) {
		game.players[index].money -= game.bet;
		game.betPlayerId = getNextPlayer();
		if(checkStateChange(id) == 0){
			//send call change
		}
		return 1;
	}
	return 0;
}

char actionRise(int id, int riseCount){
	int index = getPlayerIndex(id);
	if(checkPlayerPermission(id, PERM_RISE, riseCount)){
		game.players[index].money -= (game.bet + riseCount);
		game.lastRisePlayerId = id;
		game.betPlayerId = getNextPlayer();
		//send rise change
		return 1;
	}
	return 0;
}

char actionFold(int id){
	int index = getPlayerIndex(id);
	if(checkPlayerPermission(id, PERM_FOLD, 0)){
		game.players[index].state = PLAYER_PASS;
		game.betPlayerId = getNextPlayer();
		if(checkStateChange(id) == 0){
			//send fold change
		}
		return 1;
	}
	return 0;
}

char actionCheck(int id) {
	int index = getPlayerIndex(id);
	if(checkPlayerPermission(id, PERM_CHECK, 0)){
		game.betPlayerId = getNextPlayer();
		if(checkStateChange(id) == 0){
			//send check change
		}
		return 1;
	}
	return 0;
}

char actionAllIn(int id) {
	int index = getPlayerIndex(id);
	if(checkPlayerPermission(id, PERM_ALL_IN, 0)){
		game.bank += game.players[index].money;
		game.players[index].money = 0;
		game.players[index].state = PLAYER_PASSIVE;
		if(game.wasRise == 1){
			if(checkStateChange(id) == 0){
				//all in check send
			}
		}else{
			//send allin check
		}
		return 1;
	}
	return 0;
}

char actionConnectRequest(unsigned int session) {
	int i;
	int id = -1;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].session == session &&
				game.players[i].state == PLAYER_CONNECTING)
		{
			game.players[i].state = PLAYER_PASSIVE;
			id = game.players[i].id;
			//send player state changed
			break;
		}
	}
	return id;
}

void initGameLogic() {
	int i;
	memset(&game, 0, sizeof(struct gameState_t));
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		game.players[i].state = PLAYER_FREE;
	}
	game.state = GAME_START;
}

void newEvent(unsigned int id, unsigned char type, char sourceType, void *data){
	if(sourceType == CLIENT){ //Клиент
		switch(type) {
			case(ACTION_EXIT):
				//send player diconected to server
				//network delete player
				removePlayer(*((int *)data));
				//send PLAYER_DISCONNECTED to clients
				break;
			case(ACTION_CALL):
				if(actionCall(id) == 0){
					//send error
				}
				break;
			case(ACTION_RISE):
				if(actionRise(id, *((int *)data)) == 0){
					//send error
				}
				break;
			case(ACTION_FOLD):
				if(actionFold(id) == 0){
					//send error
				}
				break;
			case(ACTION_CHECK):
				if(actionCheck(id) == 0){
					//erorr
				}
				break;
			case(ACTION_ALL_IN):
				if(actionAllIn(id)){
					//error
				}
				break;
			case(ACTION_CONNECT_REQUEST):
				if(actionConnectRequest(*((unsigned int *)data)) == -1){
					//error
				}
				break;
			default:
				//ERROR
				break;
		}
	}
	if(sourceType == SERVER){
		switch(type) {
			case (INTERNAL_NEW_PLAYER):
				addNewPlayer((struct newPlayer_t *)data);
				//send PLAYER_NEW_PLAYER to clients
				break;
			default:
				//ERROR
				break;
		}
	}
}
