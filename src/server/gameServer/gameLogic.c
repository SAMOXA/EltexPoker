#include "gameLogic.h"

#define MAX_TAKED_CARDS 5 + 2 * MAX_PLAYERS_PER_TABLE

enum permissionsTypes_e {
	PERM_CHECK,
	PERM_RISE,
	PERM_CALL,
	PERM_ALL_IN,
	PERM_FOLD
};

unsigned char takedCard[MAX_TAKED_CARDS];

struct errorMsg_t lastError;

static char checkTakedCard(unsigned char card){
	int i;
	for(i=0;i<MAX_TAKED_CARDS;i++){
		if(takedCard[i] == card){
			return 0;
		}
	}
	return 1;
}

static unsigned char getCard(){
	unsigned char card;
	int i;
	do{
		card = rand() % 52;//TODO make less magic
	}while(checkTakedCard(card) != 1);
	for(i=0;i<MAX_TAKED_CARDS;i++){
		if(takedCard[i] == FALSE_CARD){
			takedCard[i] = card;
			break;
		}
	}
	return card;
}

static void resetCards(){
	memset(takedCard, FALSE_CARD, sizeof(unsigned char) * MAX_TAKED_CARDS);
}

static int getPlayerIndex(int id) {
	int i;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].id == id){
			return i;
		}
	}
	return -1;
}

static char playerIsActive(int id) {
	int index = getPlayerIndex(id);
	return game.players[index].state == PLAYER_ACTIVE;
}

//TODO pending buffer
static void addNewPlayer(struct newPlayer_t *newPlayer) {
	int i;
	printf("[gameLogic] addNewPlayer\n");
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].state == PLAYER_FREE){
			break;
		}
	}
	game.players[i].state = PLAYER_CONNECTING;
	game.players[i].id = newPlayer->id;
	printf("id %d\n", newPlayer->id);
	game.players[i].money = newPlayer->money;
	game.players[i].session = newPlayer->session;
	memcpy(game.players[i].name, newPlayer->name, sizeof(char) * MAX_NAME_LENGTH);
}


static char getNextPlayer(int oldPlayerId) { //Call only after reset player states
	int newIndex = oldPlayerId;
	while(1){
		newIndex = (newIndex + 1) % MAX_PLAYERS_PER_TABLE;
		if(playerIsActive(game.players[newIndex].id)){
			return newIndex;
		}
	}
}

static char checkActionPermission(int playerId, char actionType, void *data){
	int index = getPlayerIndex(playerId);
	int riseCount = *((int *)data);
	if(game.state == GAME_START || game.state == GAME_FINAL){
		lastError.type = ERROR_PERMISSION;
		strcpy(lastError.msg, "You can not do this now");
		return 0;
	}
	if(game.activePlayerId != playerId){
		lastError.type = ERROR_PERMISSION;
		strcpy(lastError.msg, "You can not do this now");
		return 0;
	}
	switch(actionType){
		case(PERM_CHECK):
			if(game.state == GAME_PRE_FLOP_ROUND){
				if(game.lastRisePlayerId != playerId){
					lastError.type = ERROR_RISE;
					strcpy(lastError.msg, "You can not pass in pre flop");
					return 0;
				}
			}else{
				if(game.lastRisePlayerId != -1 && game.lastRisePlayerId != playerId){
					lastError.type = ERROR_RISE;
					strcpy(lastError.msg, "You can not pass after rise");
					return 0;
				}
			}
			break;
		case(PERM_FOLD):
		case(PERM_ALL_IN):
			break;
		case(PERM_RISE):
			if(game.players[index].money - (game.bet + riseCount) < 0){
				lastError.type = ERROR_MONEY;
				strcpy(lastError.msg, "You have insufficient funds");
				return 0;
			}
			break;
		case(PERM_CALL):
			if(game.players[index].money - game.bet < 0){
				lastError.type = ERROR_MONEY;
				strcpy(lastError.msg, "You have insufficient funds");
				return 0;
			}
			break;
	}
	return 1;
}

static char checkWin() {
	char activePlayersCount = 0;
	int i;
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++) {
		if(playerIsActive(game.players[i].id)){
			activePlayersCount++;
		}
	}
	return activePlayersCount == 1;
}

static char checkNextTurn() {
	if(game.activePlayerId == game.lastRisePlayerId){
		return 1;
	}
	return 0;
}

static void startNewGame(){
	int i;
	game.bank = 0;
	for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
		if(game.players[i].state == PLAYER_PASS){
			game.players[i].state = PLAYER_ACTIVE;
		}
	}
	game.dillerId = getNextPlayer(game.dillerId);
	game.smallBlindPlayerId = getNextPlayer(game.dillerId);
	game.bigBlindPlayerId = getNextPlayer(game.smallBlindPlayerId);
	game.bet = 2;//TODO settings
	game.lastRisePlayerId = game.bigBlindPlayerId;
	game.lastRiseCount = 0;
	resetCards();
	memset(game.cards, FALSE_CARD, sizeof(unsigned char)*5);

	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		game.players[i].cards[0] = getCard();
		game.players[i].cards[1] = getCard();
		//memset(game.players[i].cards, FALSE_CARD, sizeof(unsigned char)*2);
		game.players[i].bet = 0;
		send_message(CLIENT, game.players[i].id, STATE_NEW_CARD_PALAYER,
				sizeof(unsigned char)*2, &game.players[i].cards);
	}
	//Blinds
	//TODO check money
	game.players[game.smallBlindPlayerId].money -= game.bet/2;
	game.players[game.smallBlindPlayerId].bet = game.bet/2;
	game.players[game.bigBlindPlayerId].money -= game.bet;
	game.bank += game.bet/2 + game.bet;

	game.state = GAME_PRE_FLOP_ROUND;
}

static void updateTurn() {
	int winIndex = -1;
	int i;
	int index;
	if(game.state == GAME_INSTANT_WIN){
		game.state = GAME_FINAL;
	}else{
		game.state++;
	}
	switch(game.state){
		case(GAME_FLOP_ROUND):
			game.cards[0] = getCard();
			game.cards[1] = getCard();
			game.cards[2] = getCard();
			break;
		case(GAME_TERN_ROUND):
			game.cards[3] = getCard();
			break;
		case(GAME_RIVER_ROUND):
			game.cards[4] = getCard();
			break;
		case(GAME_FINAL):
			winIndex = getPlayerIndex(getWinIndex());
			game.players[winIndex].money += game.bank;
			startNewGame();
	}
	if(game.state != GAME_FINAL){
		game.lastRiseCount = 0;
		game.lastRisePlayerId = game.smallBlindPlayerId;
		game.activePlayerId = game.smallBlindPlayerId;
		for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
			game.players[i].bet = 0;
		}
	}
}

static void updateTradeTurn(){
	game.activePlayerId = getNextPlayer(game.activePlayerId);
}

static void updateState() {
	if(game.state == GAME_START){
		if(game.playersCount > 1){
			startNewGame();
		}
	}
	if(game.playersCount < 2){
		game.state = GAME_START;
		send_message(ALL_CLIENTS, 0, STATE_FULL_UPDATE,
				sizeof(struct gameState_t), &game);
		return;
	}
	if(checkWin() || checkNextTurn()){
		if(checkWin()){
			game.state = GAME_INSTANT_WIN;//Instant win
		}
		updateTurn();
	}
	updateTradeTurn();
}

static char actionCall(int id){
	int index = getPlayerIndex(id);
	int betCount;
	printf("[gameLogic] actionCall %d ", id);
	if(checkActionPermission(id, PERM_CALL, 0)) {
		printf("sucsess\n");
		betCount = game.bet - game.players[index].bet;
		game.players[index].money -= betCount;
		game.players[index].bet = game.bet;
		game.bank += betCount;
		updateState();
		return 1;
	}
	printf("fail\n");
	return 0;
}

static char actionRise(int id, int riseCount){
	int index = getPlayerIndex(id);
	int betCount;
	printf("[gameLogic] actionRise %d %d ", id, riseCount);
	if(checkActionPermission(id, PERM_RISE, &riseCount)){
		printf("sucsess\n");
		betCount = (game.bet - game.players[index].bet) + riseCount;
		game.players[index].money -= betCount;
		game.bank += betCount;
		game.lastRisePlayerId = id;
		game.bet += riseCount;
		game.players[index].bet = game.bet;
		updateState();
		return 1;
	}
	printf("fail\n");
	return 0;
}

static char actionFold(int id){
	int index = getPlayerIndex(id);
	printf("[gameLogic] actionFold %d ", id);
	if(checkActionPermission(id, PERM_FOLD, 0)){
		printf("sucsess\n");
		game.players[index].state = PLAYER_PASS;
		updateState();
		return 1;
	}
	printf("fail\n");
	return 0;
}

static char actionCheck(int id) {
	int index = getPlayerIndex(id);
	printf("[gameLogic] actionCheck %d ", id);
	if(checkActionPermission(id, PERM_CHECK, 0)){
		printf("sucsess\n");
		updateState();
		return 1;
	}
	printf("fail\n");
	return 0;
}

static char actionAllIn(int id) {
	int index = getPlayerIndex(id);
	printf("[gameLogic] actionAllIn %d ", id);
	if(checkActionPermission(id, PERM_ALL_IN, 0)){
		printf("sucsess\n");
		game.bank += game.players[index].money;
		game.players[index].bet += game.players[index].money;
		game.bet += game.players[index].money;
		game.players[index].money = 0;
		updateState();
		return 1;
	}
	printf("fail\n");
	return 0;
}

static char actionConnectRequest(unsigned int session) {
	int i;
	int id = -1;
	printf("[gameLogic] actionConnectRequest %d ", session);
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		if(game.players[i].session == session &&
				game.players[i].state == PLAYER_CONNECTING)
		{
			game.players[i].state = PLAYER_PASS;
			id = game.players[i].id;
			add_id_to_table(0, id);
			game.playersCount++;
			//updateState();
			send_message(LOBBY_SERVER, 0, INTERNAL_PLAYER_CONFIRMED,
					sizeof(int), &id);
			send_message(ALL_CLIENTS, 0, STATE_NEW_PLAYER, sizeof(struct player_t),
					&(game.players[i]));
			send_message(ALL_CLIENTS, 0, STATE_FULL_UPDATE,
					sizeof(struct gameState_t), &game);
			break;
		}
	}
	if(id == -1){
		lastError.type = ERROR_AUTH;
		strcpy(lastError.msg, "Session invalid");
		printf("fail\n");
		return -1;
	}
	printf("sucsess\n");
	return id;
}

static void removePlayer(int id){
	printf("[gameLogic] Remove player %d\n", id);
	int index = getPlayerIndex(id);
	memset(&game.players[index], 0, sizeof(struct player_t));
	game.players[index].state = PLAYER_FREE;
	game.playersCount--;
	send_message(ALL_CLIENTS, 0, STATE_FULL_UPDATE,
			sizeof(struct gameState_t), &game);
	//updateState();
}

void initGameLogic() {
	int i;
	memset(&game, 0, sizeof(struct gameState_t));
	for(i=0; i<MAX_PLAYERS_PER_TABLE; i++){
		game.players[i].state = PLAYER_FREE;
	}
	game.state = GAME_START;
	srand(time(NULL));
	resetCards();
}

void gameEvents(int sourceType, int id, int type, void *data){
	char errorFlag = 0;
	printf("[gameLogic] gameEvents\n");
	if(sourceType == CLIENT){ //Клиент
		switch(type) {
			case(ACTION_EXIT):
				removePlayer(id);
				//updateState();
				del_id_from_table(0, id);
				send_message(LOBBY_SERVER, 0, INTERNAL_PLAYER_LEFT, sizeof(int), &id);
				send_message(ALL_CLIENTS, 0, STATE_PLAYER_DISCONECTED,
						sizeof(int), &id);
				send_message(ALL_CLIENTS, 0, STATE_FULL_UPDATE,
						sizeof(struct gameState_t), &game);
				break;
			case(ACTION_CALL):
				if(actionCall(id) == 0){
					errorFlag = 1;
				}
				break;
			case(ACTION_RISE):
				if(actionRise(id, *((int *)data)) == 0){
					errorFlag = 1;
				}
				break;
			case(ACTION_FOLD):
				if(actionFold(id) == 0){
					errorFlag = 1;
				}
				break;
			case(ACTION_CHECK):
				if(actionCheck(id) == 0){
					errorFlag = 1;
				}
				break;
			case(ACTION_ALL_IN):
				if(actionAllIn(id)){
					errorFlag = 1;
				}
				break;
			case(ACTION_CONNECT_REQUEST):
				if(actionConnectRequest(*((unsigned int *)data)) == -1){
					*((char *)data) = 0;
					errorFlag = 1;
				}
				*((char *)data) = 1;
				break;
			default:
				errorFlag = 1;
				break;
		}
		if(errorFlag == 0){
			send_message(ALL_CLIENTS, 0, STATE_FULL_UPDATE,
					sizeof(struct gameState_t), &game);
		}else{
			send_message(CLIENT, id, STATE_ERROR,
					sizeof(struct errorMsg_t), &lastError);
		}
	}
	if(sourceType == LOBBY_SERVER){
		switch(type) {
			case (INTERNAL_NEW_PLAYER):
				addNewPlayer((struct newPlayer_t *)data);
				//add_id_to_table(0, id);
				break;
			default:
				//ERROR
				break;
		}
	}
}
