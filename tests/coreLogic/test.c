#include <stdio.h>
#include "test.h"

struct testStruct_t tests[3] = {
	{
		.cards={
			CREATE_CARD(TWO, HEARTS),
			CREATE_CARD(THREE, DIAMONDS),
			CREATE_CARD(FOUR, CLUBS),
			CREATE_CARD(QUEEN, SPADES),
			CREATE_CARD(KING, HEARTS)
		},
		.playersCard={
			{
				CREATE_CARD(FIVE, DIAMONDS),
				CREATE_CARD(SEVEN, CLUBS)
			},
			{
				CREATE_CARD(FIVE, SPADES),
				CREATE_CARD(SEVEN, HEARTS)
			},
			{
				CREATE_CARD(FIVE, CLUBS),
				CREATE_CARD(SEVEN, DIAMONDS)
			}
		},
		.expectWinners={-1, -1, -1}
	},
	{
		.cards={
			CREATE_CARD(TWO, HEARTS),
			CREATE_CARD(THREE, HEARTS),
			CREATE_CARD(FOUR, HEARTS),
			CREATE_CARD(QUEEN, SPADES),
			CREATE_CARD(KING, DIAMONDS)
		},
		.playersCard={
			{
				CREATE_CARD(FIVE, DIAMONDS),
				CREATE_CARD(SEVEN, CLUBS)
			},
			{
				CREATE_CARD(FIVE, HEARTS),
				CREATE_CARD(SEVEN, HEARTS)
			},
			{
				CREATE_CARD(FIVE, CLUBS),
				CREATE_CARD(SEVEN, DIAMONDS)
			}
		},
		.expectWinners={1, -1, -1}
	},
	{
		.cards={
			CREATE_CARD(TWO, HEARTS),
			CREATE_CARD(THREE, HEARTS),
			CREATE_CARD(FOUR, HEARTS),
			CREATE_CARD(QUEEN, HEARTS),
			CREATE_CARD(KING, HEARTS)
		},
		.playersCard={
			{
				CREATE_CARD(FIVE, DIAMONDS),
				CREATE_CARD(SEVEN, CLUBS)
			},
			{
				CREATE_CARD(FIVE, SPADES),
				CREATE_CARD(SEVEN, DIAMONDS)
			},
			{
				CREATE_CARD(FIVE, CLUBS),
				CREATE_CARD(SEVEN, DIAMONDS)
			}
		},
		.expectWinners={0, 1, 2}
	}
};

int testsCount = 3;

void printCard(unsigned char card){
	char lear = GET_CARD_LEAR(card);
	char value = GET_CARD_VALUE(card);
	switch(value){
		case(TWO):
		case(THREE):
		case(FOUR):
		case(FIVE):
		case(SIX):
		case(SEVEN):
		case(EIGHT):
		case(NINE):
		case(TEN):
			printf("%d", value+2);
			break;
		case(JACK):
			printf("J");
			break;
		case(QUEEN):
			printf("Q");
			break;
		case(KING):
			printf("K");
			break;
		case(ACE):
			printf("A");
			break;
	}
	switch(lear){
		case(HEARTS):
			printf("h");
			break;
		case(DIAMONDS):
			printf("d");
			break;
		case(CLUBS):
			printf("c");
			break;
		case(SPADES):
			printf("s");
			break;
	}
	printf(" ");
}

int main(){
	int i, j;
	for(i=0;i<3;i++){
		game.players[i].state = PLAYER_ACTIVE;
	}
	for(i=0;i<testsCount;i++){
		for(j=0;j<5;j++){
			game.cards[j] = tests[i].cards[j];
			printCard(game.cards[j]);
		}
		printf(" \t");
		for(j=0;j<3;j++){
			game.players[j].cards[0] = tests[i].playersCard[j][0];
			printCard(game.players[j].cards[0]);
			game.players[j].cards[1] = tests[i].playersCard[j][1];
			printCard(game.players[j].cards[1]);
			printf("\t");
		}
		getWinIndex();
		printf("\nExpect winners {");
		for(j=0;j<3;j++){
			printf("%d ", tests[i].expectWinners[j]);
		}
		printf("} actual winners {");
		for(j=0;j<3;j++){
			printf("%d ", winners[j]);
		}
		printf("}");
		if(memcmp(winners, tests[i].expectWinners, sizeof(int)*3) != 0){
			printf(" \t[FAIL]\n");
			return -1;
		}else{
			printf(" \t[PASS]\n");
		}
	}
	return 0;
}
