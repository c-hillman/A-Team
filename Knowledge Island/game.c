#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Game.h"

#define NUMBER_OF_DISCIPLINES 6
#define NUMBER_OF_HEXES 19

#define DEFAULT_DISCIPLINES {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, \
                STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, \
                STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS }
#define DEFAULT_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5}

typedef struct _university {
	int KPI;
	int noCampuses, noGO8s;
	int students[NUMBER_OF_DISCIPLINES];
	// stores how many students of each type a university has
	// discipline type code corresponds to index
	// ie. STUDENT_THD stored at students[0] etc.
	
	int exchange[NUMBER_OF_DISCIPLINES];
	// stores whether or not (no = 0, yes = 1) a
	// university has a special exchange rate (2 for 1)
	// discipline type code corresponds to index
	// ie. STUDENT_THD stored at students[0] etc.
	
	int noPatent, noARCs, noPaper;
} university;

typedef struct _game { 
	int discipline[NUMBER_OF_HEXES];
	int dice[NUMBER_OF_HEXES];
	int currentTurn;
	int diceScore;
	university uni[NUM_UNIS];
} game;

int main (int argc, char *argv[]) {
	
	return EXIT_SUCCESS;
}

Game newGame(int discipline[], int dice[]) {
	printf("Starting newGame\n");
	Game g = (Game) malloc(sizeof game);

	int i = 0;
	while (i < NUMBER_OF_HEXES) {
		g->discipline[i] = discipline[i];
		g->dice[i] = dice[i];
		i++;
	}

	g->currentTurn = -1;
	g->diceScore = 0;

	university u = {0,
					2, 0,
					{0, 3, 3, 1, 1, 1},
					{0, 0, 0, 0, 0, 0},
					0, 0, 0};

	i = 0;
	while (i < NUM_UNIS) {
		g -> uni[i] = u;
		i++;
	}
	
	return g;
}

void disposeGame(Game g) {
	free(g);
}

void makeAction(Game g, action a) {
	if (a.actionCode == 0) {
		int d = (rand() % 6 + 1);
		d += (rand() % 6 + 1);
		throwDice(g, d);
	} else if (a.actionCode == 1) {
		//DO SOMETHING
	} else if (a.actionCode == 2) {
		//DO SOMETHING
	} else if (a.actionCode == 3) {
		//DO SOMETHING
	} else if (a.actionCode == 4) {
		//DO SOMETHING
	} else if (a.actionCode == 5) {
		//DO SOMETHING
	} else if (a.actionCode == 6) {
		//DO SOMETHING
	} else if (a.actionCode == 7) {
		//DO SOMETHING
	}
}


void throwDice(Game g, int diceScore) {
	g->currentTurn++;
	g->diceScore = diceScore;
}

/*
int getDiscipline(Game g, int regionID) {

}

int getDiceValue(Game g, int regionID) {

}*/