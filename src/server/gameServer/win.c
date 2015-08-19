#include "win.h"

#define CARDS_COUNT 5 + 2

struct cardPerLear_t {
	unsigned char cards[CARDS_COUNT];
	int count;
	};

struct combos_t {
	unsigned char combo[5];
	unsigned char cards[CARDS_COUNT];
	struct cardPerLear_t lears[4];
	unsigned char playerId;
} combos[MAX_PLAYERS_PER_TABLE];

char prepareToWin[MAX_PLAYERS_PER_TABLE];
int prepareToWinCount = 0;

int winners[MAX_PLAYERS_PER_TABLE];

unsigned char activePlayersCount = 0;

int compare(const void *elem1, const void *elem2){
	unsigned char *card1 = (unsigned char *)elem1;
	unsigned char *card2 = (unsigned char *)elem2;
	if(*card1 < *card2){
		return -1;
	}
	if(*card2 < *card1){
		return 1;
	}
	return 0;
}

char checkCard(char value, char lear, unsigned char *cards, int count){
	int i;
	for(i=0;i<count;i++){
		if(GET_CARD_VALUE(cards[i]) == value &&
				(lear == -1 || GET_CARD_LEAR(cards[i]) == lear)){
			return 1;
		}
	}
	return 0;
}

char getCardByValue(char value, unsigned char *cards, int count){
	int i;
	for(i=0;i<count;i++){
		if(GET_CARD_VALUE(cards[i]) == value){
			return cards[i];
		}
	}
	return -1;
}

void copyStrait(unsigned char *dest, unsigned char *source, char ace){
	int counter = 1 + ace;
	unsigned char old;
	*dest = *source;
	old = *source;
	dest--;
	source--;
	while(counter < 5){
		if(old != *source){
			*dest = *source;
			dest--;
			counter++;
		}
		source--;
	}
}

char checkRoyalFlush(int index){
	int lear, value;
	char foundFlag;
	int i;
	for(lear=HEARTS;lear<=SPADES;lear++){
		if(combos[index].lears[lear].count >= 5){
			foundFlag = 1;
			for(value=TEN;value<=ACE;value++){
				if(checkCard(value, lear, combos[index].lears[lear].cards,
							combos[index].lears[lear].count) == 0){
					foundFlag = 0;
					break;
				}
			}
			if(foundFlag == 1){
				for(i=0;i<5;i++){
					combos[index].combo[i] = CREATE_CARD(TEN+i,lear);
				}
				return 1;
			}
		}
	}
	return 0;
}

unsigned char* getStraight(unsigned char *cards, int count){
	unsigned char curentValue;
	char foundFlag;
	static unsigned char combo[5];
	unsigned char aceCard;
	int FIVE_index;
	int i, j;

	for(i=0;i<count-4;i++){
		curentValue = GET_CARD_VALUE(cards[count-i-1]);
		foundFlag = 1;
		for(j=1;j<5;j++){
			if(curentValue == GET_CARD_VALUE(cards[count-i-j-1])){
				continue;
			}
			if(curentValue - 1 != GET_CARD_VALUE(cards[count-i-j-1])){
				foundFlag = 0;
				break;
			}else{
				curentValue--;
			}
		}
		if(foundFlag == 1){
			copyStrait(combo+5, &(cards[count-i-1]), 0);
			return combo;
		}
	}
	if(checkCard(ACE, -1, cards, count) && checkCard(TWO, -1, cards, count)){
		curentValue = GET_CARD_VALUE(cards[0]);
		foundFlag = 1;
		for(i=1;i<3;i++){
			if(curentValue == GET_CARD_VALUE(cards[i])){
				continue;
			}
			if(GET_CARD_VALUE(cards[i]) == FIVE){
				FIVE_index = i;
			}
			if(curentValue + 1 != GET_CARD_VALUE(cards[i])){
				foundFlag = 0;
				break;
			}else{
				curentValue--;
			}
		}
		if(foundFlag == 1){
			aceCard = getCardByValue(ACE, cards, count);
			combo[0] = aceCard;
			copyStrait(combo+5, &(cards[FIVE_index]), 1);
			return combo;
		}
	}
	return 0;
}

char checkPlayerFlush(int index){
	int lear;
	int learCount;
	for(lear=HEARTS;lear<=SPADES;lear++){
		learCount = combos[index].lears[lear].count;
		if(learCount >= 5){
			prepareToWin[prepareToWinCount] = index;
			memcpy(combos[index].combo, &(combos[index].lears[lear].cards[learCount-5]),
					sizeof(unsigned char)*5);
			prepareToWinCount++;
			return 1;
		}
	}
	return 0;
}

char checkFlush(){
	int playerIndex;
	unsigned char higestFlush[5] = {0,0,0,0,0};
	int higestFlushPlayerIndex[activePlayersCount];
	int currentPlayerIndex = 0;
	int currentIndex;
	memset(higestFlushPlayerIndex, 0, sizeof(int)*activePlayersCount);
	for(playerIndex=0;playerIndex<activePlayersCount;playerIndex++){
		if(checkPlayerFlush(playerIndex) == 1){
			currentIndex = 4;
			while(1){
				if(combos[prepareToWinCount - 1].combo[currentIndex] ==
						higestFlush[currentIndex])
				{
					currentIndex--;
					if(currentIndex == -1){
						higestFlushPlayerIndex[currentPlayerIndex] = playerIndex;
						currentPlayerIndex++;
						break;
					}
				}else{
					break;
				}
			}
			if(currentIndex != -1 && combos[prepareToWinCount - 1].combo[currentIndex] >
					higestFlush[currentIndex])
			{
				currentPlayerIndex = 1;
				higestFlushPlayerIndex[0] = playerIndex;
				memcpy(higestFlush, combos[prepareToWinCount - 1].combo,
						sizeof(unsigned char)*5);
			}
		}
	}
	memcpy(winners, higestFlushPlayerIndex, sizeof(int)*currentPlayerIndex);
	return currentPlayerIndex;
}

void preparePlayersCards(){
	int i, j;
	int currentLear;
	int learCount;
	for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
		if(game.players[i].state == PLAYER_ACTIVE){
			combos[activePlayersCount].playerId = game.players[i].id;
			memcpy(&(combos[activePlayersCount].cards[5]), game.players[i].cards,
					sizeof(unsigned char)*2);
			activePlayersCount++;
		}
	}
	for(i=0;i<activePlayersCount;i++){
		memcpy(combos[i].cards, game.cards, sizeof(unsigned char)*5);
		qsort(combos[i].cards, CARDS_COUNT, sizeof(unsigned char), compare);
	}
	for(i=0;i<activePlayersCount;i++){
		for(j=0;j<7;j++){
			currentLear = GET_CARD_LEAR(combos[i].cards[j]);
			learCount = combos[i].lears[currentLear].count;
			combos[i].lears[currentLear].cards[learCount] = combos[i].cards[j];
			combos[i].lears[currentLear].count++;
		}
	}
}

char getWinIndex() {
	unsigned char cards[CARDS_COUNT];
	memset(winners, -1, sizeof(int) * MAX_PLAYERS_PER_TABLE);
	memset(combos, 0, sizeof(struct combos_t)*MAX_PLAYERS_PER_TABLE);
	activePlayersCount = 0;
	preparePlayersCards();
	prepareToWinCount = 0;
	checkFlush();
	return combos[rand()%activePlayersCount].playerId;
}

/*
int main(){
	int i;
	game.cards[0] = CREATE_CARD(TEN, SPADES);
	game.cards[1] = CREATE_CARD(JACK, SPADES);
	game.cards[2] = CREATE_CARD(QUEEN, SPADES);
	game.cards[3] = CREATE_CARD(KING, SPADES);
	game.cards[4] = CREATE_CARD(SEVEN, SPADES);
	game.players[0].id = 0;
	game.players[0].state = PLAYER_ACTIVE;
	game.players[0].cards[0] = CREATE_CARD(TWO, SPADES);
	game.players[0].cards[1] = CREATE_CARD(THREE, SPADES);
	game.players[1].id = 1;
	game.players[1].state = PLAYER_ACTIVE;
	game.players[1].cards[0] = CREATE_CARD(FOUR, SPADES);
	game.players[1].cards[1] = CREATE_CARD(FIVE, SPADES);
	game.players[2].id = 2;
	game.players[2].state = PLAYER_ACTIVE;
	game.players[2].cards[0] = CREATE_CARD(SIX, SPADES);
	game.players[2].cards[1] = CREATE_CARD(ACE, SPADES);
	getWinIndex();
	for(i=0;i<activePlayersCount;i++){
		printf("%d ", winners[i]);
	}
	printf("\n");

	game.cards[0] = CREATE_CARD(TWO, SPADES);
	game.cards[1] = CREATE_CARD(FOUR, HEARTS);
	game.cards[2] = CREATE_CARD(ACE, CLUBS);
	game.cards[3] = CREATE_CARD(KING, HEARTS);
	game.cards[4] = CREATE_CARD(ACE, HEARTS);
	game.players[0].id = 0;
	game.players[0].state = PLAYER_ACTIVE;
	game.players[0].cards[0] = CREATE_CARD(FIVE, HEARTS);
	game.players[0].cards[1] = CREATE_CARD(THREE, SPADES);
	game.players[1].id = 1;
	game.players[1].state = PLAYER_ACTIVE;
	game.players[1].cards[0] = CREATE_CARD(SEVEN, HEARTS);
	game.players[1].cards[1] = CREATE_CARD(TEN, HEARTS);
	game.players[2].id = 2;
	game.players[2].state = PLAYER_ACTIVE;
	game.players[2].cards[0] = CREATE_CARD(SIX, HEARTS);
	game.players[2].cards[1] = CREATE_CARD(QUEEN, HEARTS);
	getWinIndex();
	for(i=0;i<activePlayersCount;i++){
		printf("%d ", winners[i]);
	}
	printf("\n");

	game.cards[0] = CREATE_CARD(TEN, HEARTS);
	game.cards[1] = CREATE_CARD(JACK, HEARTS);
	game.cards[2] = CREATE_CARD(QUEEN, HEARTS);
	game.cards[3] = CREATE_CARD(KING, HEARTS);
	game.cards[4] = CREATE_CARD(NINE, HEARTS);
	game.players[0].id = 0;
	game.players[0].state = PLAYER_ACTIVE;
	game.players[0].cards[0] = CREATE_CARD(FIVE, SPADES);
	game.players[0].cards[1] = CREATE_CARD(THREE, SPADES);
	game.players[1].id = 1;
	game.players[1].state = PLAYER_ACTIVE;
	game.players[1].cards[0] = CREATE_CARD(SEVEN, SPADES);
	game.players[1].cards[1] = CREATE_CARD(TEN, SPADES);
	game.players[2].id = 2;
	game.players[2].state = PLAYER_ACTIVE;
	game.players[2].cards[0] = CREATE_CARD(SIX, SPADES);
	game.players[2].cards[1] = CREATE_CARD(QUEEN, SPADES);
	getWinIndex();
	for(i=0;i<activePlayersCount;i++){
		printf("%d ", winners[i]);
	}
	printf("\n");
	return 0;
}
*/
