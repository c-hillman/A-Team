#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Game.h"

#define NUMBER_OF_DISCIPLINES 6
#define NUMBER_OF_HEXES 19

#define DEFAULT_DISCIPLINES {STUDENT_MJ,  STUDENT_MMONEY, STUDENT_BQN, \
					 STUDENT_MTV, STUDENT_BPS, STUDENT_MJ, STUDENT_MMONEY, \
					 STUDENT_MJ, STUDENT_BQN, STUDENT_MTV, STUDENT_BPS, \
					 STUDENT_MTV, STUDENT_MMONEY, STUDENT_MJ, STUDENT_THD, \
					 STUDENT_BQN, STUDENT_BPS, STUDENT_BQN, STUDENT_MTV }
#define DEFAULT_DICE {8,10,9,3,5,6,12,6,4,11,3,11,2,9,7,4,5,10,8}

#define MAP_VALUES {{9,9,9,9,9,0,1,9,9,9,9,9}, \
					{9,9,9,0,0,8,8,0,0,9,9,9}, \
					{9,3,0,8,8,0,0,8,8,0,0,9}, \
					{0,8,8,0,0,8,8,0,0,8,8,2}, \
					{9,0,0,8,8,0,0,8,8,0,0,9}, \
					{0,8,8,0,0,8,8,0,0,8,8,0}, \
					{9,0,0,8,8,0,0,8,8,0,0,9}, \
					{2,8,8,0,0,8,8,0,0,8,8,0}, \
					{9,0,0,8,8,0,0,8,8,0,3,9}, \
					{9,9,9,0,0,8,8,0,0,9,9,9}, \
					{9,9,9,9,9,1,0,9,9,9,9,9}} 
// 0 = Vacant Vertex
// 8 = Center of a Region
// 9 = Illegal Vertex

#define MAX_ARCS 25
#define SIZE_VECTOR 4 
#define MAX_GO8 8

#define HORIZONTAL_VERTICES	12
#define VERTICAL_VERTICES	11

#define X_1 0
#define Y_1 1
#define X_2 2
#define Y_2 3

static int highestNumber(int intA, int intB, int intC);
static int legalSpinoff(int noMj,int noMtv,int noMmoney);
static int legalRetrain(Game g ,int actvPlayer,action a,int exchangeRate);
static int legalCampus(Game g,int vector[],int noBps,int noBqn,int noMj,int noMtv);
static int legalGO8(Game g,int actvPlayer,int vector[],int noMj,int noMmoney);
static int legalARC(Game g, action a,int actvPlayer,int vector[],int noBps,int noBqn);
static int adjARCs(Game g,action a, int actvPlayer, int vector[]);
static int isLegalPath(Game g, int x2, int y2);
static void lostArc(Game g, path path, int lastVector[]);
static void generateDisciplines(Game g, int diceScore);
static int checkAdjUni(Game g,int x, int y, int uni);

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

	int arcs[MAX_ARCS][SIZE_VECTOR];
} university;

typedef struct _game { 
	int discipline[NUMBER_OF_HEXES];
	int dice[NUMBER_OF_HEXES];
	int currentTurn;
	int diceScore;
	int mostARCs;
	int mostPublications;
	int map[VERTICAL_VERTICES][HORIZONTAL_VERTICES];
	university uni[NUM_UNIS];
} game;


Game newGame(int discipline[], int dice[]) {
	printf("Starting newGame\n");

	int mapVal[VERTICAL_VERTICES][HORIZONTAL_VERTICES] = MAP_VALUES;

	Game g = (Game) malloc(sizeof (game));

	//??
	int i = 0;
	while (i < NUMBER_OF_HEXES) {
		g->discipline[i] = discipline[i];
		g->dice[i] = dice[i];
		i++;
	}
	
	//assigns generators to regions (within our map)
    //make function: void randomRegions(void);
	//can be stored instead of the 9's inside the hexagonals(two spaces)
	//must contain dice value & discipline
	
	g->currentTurn = -1;
	g->diceScore = 0;
	g->mostARCs = 0;
	g->mostPublications = 0;
	//reset map
	i = 0;
	while (i < VERTICAL_VERTICES){
		int j = 0;
		while(j < HORIZONTAL_VERTICES){
			g->map[i][j] = mapVal[i][j];
			j++;
		}
		i++;
	}
	university u = {20,
					2, 0,
					{0, 3, 3, 1, 1, 1},
					{0, 0, 0, 0, 0, 0},
					0, 0, 0};
	//reset arcs - only for 1 uni?
	i = 0;
	while (i < VERTICAL_VERTICES){
		u.arcs[i][0] = 0;
		u.arcs[i][1] = 0;
		u.arcs[i][2] = 0;
		u.arcs[i][3] = 0;
		i++;
	}

	//??
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
	int p = getWhoseTurn(g) - 1;
	int lastVector[SIZE_VECTOR];
	int most; //only so mostFunctions can be called legally 
	most = 0;
	most = most + 0;
	
	if (a.actionCode == OBTAIN_PUBLICATION) {
		g->uni[p].students[STUDENT_MJ]--;
		g->uni[p].students[STUDENT_MTV]--;
		g->uni[p].students[STUDENT_MMONEY]--;
		
		g->uni[p].noPaper++;
		most = getMostPublications (g);
	} else if (a.actionCode == OBTAIN_IP_PATENT) {
		g->uni[p].students[STUDENT_MJ]--;
		g->uni[p].students[STUDENT_MTV]--;
		g->uni[p].students[STUDENT_MMONEY]--;
		
		g->uni[p].noPatent++;
		g->uni[p].KPI +=10;
	}else if (a.actionCode == PASS) {
		int d = (rand() % 6 + 1);
		d += (rand() % 6 + 1);
		throwDice(g, d);
	} else if (a.actionCode == BUILD_CAMPUS) {
		// cost
		g->uni[p].students[STUDENT_BPS]--;
		g->uni[p].students[STUDENT_BQN]--;
		g->uni[p].students[STUDENT_MJ]--;
		g->uni[p].students[STUDENT_MTV]--;
		
		//uniA=1, uniB=3, uniC=5
		lostArc(g, a.destination, lastVector);
		g->map[lastVector[Y_2]][lastVector[X_2]] = (p+1);
		g->uni[p].noCampuses++;
		g->uni[p].KPI +=10;
	} else if (a.actionCode == BUILD_GO8) {
		//cost
		g->uni[p].students[STUDENT_BQN]--;
		g->uni[p].students[STUDENT_BQN]--;
		g->uni[p].students[STUDENT_MMONEY]--;
		g->uni[p].students[STUDENT_MMONEY]--;
		g->uni[p].students[STUDENT_MMONEY]--;
		
		//uniA=2, uniB=4, uniC=6
		lostArc(g, a.destination, lastVector);
		g->map[lastVector[Y_2]][lastVector[X_2]] = p+4;

		g->uni[p].noGO8s++;
		g->uni[p].noCampuses--;
		g->uni[p].KPI +=10;
	} else if (a.actionCode == OBTAIN_ARC) {	
		//cost
			g->uni[p].students[STUDENT_BPS]--;
			g->uni[p].students[STUDENT_BQN]--;
		//finds last arc and stores it in first open spot
		lostArc(g, a.destination, lastVector);
		
		int counter = 0;
		while (g->uni[p].arcs[counter][X_1] != 0 \
			   && g->uni[p].arcs[counter][Y_1] != 0 \
			   && counter <= MAX_ARCS){
			counter++;
		}
		g->uni[p].arcs[counter][X_1]=lastVector[X_1];
		g->uni[p].arcs[counter][Y_1]=lastVector[Y_1];
		g->uni[p].arcs[counter][X_2]=lastVector[X_2];
		g->uni[p].arcs[counter][Y_2]=lastVector[Y_2];
		g->uni[p].noARCs++;
		most = getMostARCs(g);	
		g->uni[p].KPI +=2;
	} else if (a.actionCode == START_SPINOFF) {
		//cost
} else if (a.actionCode == RETRAIN_STUDENTS) {
		int exchange = getExchangeRate(g, p + 1, a.disciplineFrom, a.disciplineTo);
		g->uni[p].students[a.disciplineFrom] -= exchange;
		g->uni[p].students[a.disciplineTo]++;
	}	
	return;
}

void throwDice(Game g, int diceScore) {
	g->currentTurn++;
	g->diceScore = diceScore;
	
	//generates disciplines for each uni according to college location
	generateDisciplines(g, diceScore);
}


int getDiscipline(Game g, int regionID) {
	return g->discipline[regionID];
}

int getDiceValue(Game g, int regionID) {
	return g->dice[regionID];
}

int getMostARCs (Game g){
    int newMostARCs;

	int uniA = getARCs(g, UNI_A);
	int uniB = getARCs(g,UNI_B);
	int uniC = getARCs(g,UNI_C);

    if ((uniA == uniB && uniB == uniC)   \
		  ||(uniA == uniB && uniA > uniC) \
	      ||(uniC == uniB && uniC > uniA) \
		  ||(uniA == uniC && uniA > uniB)){
		newMostARCs = g->mostARCs;
	} else {
		newMostARCs = highestNumber(uniA, uniB, uniC);
	}
	
	//KPI
	if (g->mostARCs == NO_ONE && g->mostARCs != newMostARCs){
		g->mostARCs=newMostARCs;
		g->uni[g->mostARCs - 1].KPI += 10;
	} else if (newMostARCs == NO_ONE && g->mostARCs != newMostARCs){
		g->uni[g->mostARCs - 1].KPI -= 10;
		g->mostARCs=newMostARCs;
	} else if (g->mostARCs != newMostARCs){
		g->uni[g->mostARCs - 1].KPI -= 10;
		g->mostARCs=newMostARCs;
        g->uni[g->mostARCs - 1].KPI += 10;
	}
	return newMostARCs;
}

int getMostPublications (Game g){
	int newMostPublications; 
	int uniA = g->uni[0].noPaper;
	int uniB = g->uni[1].noPaper;
	int uniC = g->uni[2].noPaper;
    if ((uniA == uniB && uniB == uniC)    \
		  ||(uniA == uniB && uniA > uniC) \
	      ||(uniC == uniB && uniC > uniA) \
		  ||(uniA == uniC && uniA > uniB)){
		newMostPublications = g->mostPublications;
	} else {
		newMostPublications = highestNumber(uniA, uniB, uniC);
	}
	
	//KPI
	if (g->mostPublications == NO_ONE &&
	       g->mostPublications != newMostPublications){
		g->mostPublications=newMostPublications;
		g->uni[g->mostPublications - 1].KPI += 10;
	} else if (newMostPublications == NO_ONE && 
	           g->mostPublications != newMostPublications){
		g->uni[g->mostPublications - 1].KPI -= 10;
		g->mostPublications=newMostPublications;
	} else if (g->mostPublications != newMostPublications){
		g->uni[g->mostPublications - 1].KPI -= 10;
		g->mostPublications=newMostPublications;
        g->uni[g->mostPublications - 1].KPI += 10;
	}
	
	return newMostPublications;
}

int getTurnNumber (Game g){
	return g->currentTurn;
}

int getWhoseTurn (Game g){
	return g->currentTurn%3+1;
}


int getCampus(Game g, path pathToVertex){
	int vector[SIZE_VECTOR];
	int result = VACANT_VERTEX; 
	int vertexInfo;

	lostArc(g,pathToVertex, vector);
	vertexInfo = g->map[vector[Y_2]][vector[X_2]];
	if (vertexInfo != 0){
		if (vertexInfo == 1){
			result = UNI_A;

		} else if (vertexInfo == 2) {
			result = UNI_B;
		} else if (vertexInfo == 3) {
			result = UNI_C;
		}
	}
	return result;	
}

int getARC(Game g,path pathToEdge){
	int count = 0;
	int count2 = 0;
	int match = 0;
	int vector[4];
	int result = VACANT_ARC;
	int actvPlayer = getWhoseTurn(g);
	int noARCs = getARCs(g, actvPlayer);

	lostArc(g, pathToEdge, vector);
	while (count < noARCs){
		count2 = 0;
		match = 0;
		while (count2 < 4){
			if (g->uni[actvPlayer - 1].arcs[count][count2] == vector[count2]) {
				match++;
				if (match == 4){
					result = actvPlayer;
				}
			}
			count2++;
		}
		count++;
	}	
	return result;
}
/*
 * isLegalAction is going to test wich action the player wants to do
 * and then call a function that will return TRUE or FALSE to bol
 * whether the player is allowed or not to do the action
 */
int isLegalAction(Game g,action a){
	int bol = FALSE;
	int actvPlayer = getWhoseTurn(g);
	int noBps = getStudents(g, actvPlayer, STUDENT_BPS);
	int noBqn = getStudents(g, actvPlayer, STUDENT_BQN);
	int noMj  = getStudents(g, actvPlayer, STUDENT_MJ);
	int noMtv = getStudents(g, actvPlayer, STUDENT_MTV);
	int noMmoney = getStudents(g, actvPlayer, STUDENT_MMONEY);
	int exchangeRate = getExchangeRate(g, actvPlayer, a.disciplineFrom, a.disciplineTo); 
	int vector[4];

	lostArc(g, a.destination, vector);

	if (getTurnNumber(g) == -1) {
		bol = FALSE;
	} else if (a.actionCode == PASS){ // Always a legal action
		bol = TRUE;
	} else if (a.actionCode == OBTAIN_IP_PATENT) { // Ilegal action for a player to make
		bol = TRUE;
	} else if (a.actionCode == OBTAIN_PUBLICATION) { // Ilegal action for a player to make      
		bol = TRUE;
	} else if (a.actionCode == START_SPINOFF) {  
		bol = legalSpinoff(noMj, noMtv, noMmoney);
	} else if (a.actionCode == RETRAIN_STUDENTS) {
		bol = legalRetrain(g , actvPlayer, a, exchangeRate);
	} else if (a.actionCode == BUILD_CAMPUS) {	
		bol = legalCampus(g, vector, noBps, noBqn, noMj, noMtv);
	} else if (a.actionCode == BUILD_GO8) {
		bol = legalGO8(g, actvPlayer, vector, noMj, noMmoney);
	} else if (a.actionCode == OBTAIN_ARC) {
		bol = legalARC(g, a, actvPlayer, vector, noBps, noBqn);		
	}
	return bol;
}

int getKPIpoints (Game g, int  player){
	return g->uni[player - 1].KPI;	
}

int getARCs(Game g, int player){
	return g->uni[player - 1].noARCs;
}
int getGO8s (Game g, int player){
	return g->uni[player-1].noGO8s;
}

int getCampuses (Game g, int player){
	return g->uni[player-1].noCampuses;
}

int getIPs (Game g, int player){
	return g->uni[player-1].noPatent;
}

int getPublications (Game g, int player){
	return g->uni[player-1].noPaper;
}

int getStudents (Game g, int player, int discipline){
	return g->uni[player-1].students[discipline];
}

int getExchangeRate (Game g, int player, int disciplineFrom, int disciplineTo){
	return 3 - (g->uni[player-1].exchange[disciplineFrom]);
}
/*
 * functions to test each different Action for isLegalAction
 */
static int legalSpinoff(int noMj,int noMtv,int noMmoney){
	int bol;	
	if (noMj >= 1 && noMtv >= 1 && noMmoney >= 1) { // See if they have the required resouces
		bol = TRUE;
	} else {
		bol = FALSE;
	}
	return bol;
}

static int legalRetrain(Game g ,int actvPlayer,action a,int exchangeRate){
	int bol;
	if (getStudents(g, actvPlayer, a.disciplineFrom) >= exchangeRate && \
			a.disciplineFrom != STUDENT_THD){ // check if he has enough students and that he isnt trying to
		bol = TRUE;						   // exchange THD students.
	} else {
		bol = FALSE;
	}
	return bol;
}

static int legalCampus(Game g,int vector[],int noBps,int noBqn,int noMj,int noMtv){
	int bol = FALSE;
	int countCol = -1;
	int countRow = -1;
	if (noBps >= 1 && noBqn >= 1 && noMj >= 1 && noMtv >= 1 \
			&& vector[0] != -1){ 
		while (countCol < 2) {     
			while(countRow < 2) {
				if ((g->map[vector[Y_2] + countRow][vector[X_2] + countCol] == VACANT_VERTEX) \
						|| (g->map[vector[Y_2] + countRow][vector[X_2] + countCol] == 9)\
						|| (g->map[vector[Y_2] + countRow][vector[X_2] + countCol] == 8)) { // CHANGE INVALID
						
					countRow++;                 // to the equivalent when the place is invalid to build
				} else {

					countRow = 3;          //Both whiles are used to iterate all the adjacent vertex
					countCol = 2;          // are vacant or are invalid to build.
				}
				countCol++;
			}
			if (countCol == 3){

				bol = FALSE;
			} else {

				bol = TRUE;
			}
		} 
	}
	return bol;
}

static int legalGO8(Game g,int actvPlayer,int vector[],int noMj,int noMmoney){
	int bol;
	int go8count = 0;
	int count = 1;
	while (go8count < MAX_GO8 && count < (NUM_UNIS + 1)){ // check if there is 8 GO8 built already
		go8count = go8count + getGO8s(g, count);
		count++;	
	}	
	if (go8count < MAX_GO8 && noMj >= 2 && noMmoney >= 3 \
			&& g->map[vector[3]][vector[2]] == actvPlayer ){
		bol = TRUE;			
	} else {
		bol = FALSE;
	}
	return bol;
}

static int legalARC(Game g, action a,int actvPlayer,int vector[],int noBps,int noBqn){
	int bol = FALSE;	
	if (noBps >= 1 && noBqn >= 1 && vector[0] != -1){
		if (getARC(g, a.destination) == VACANT_VERTEX){
			if (adjARCs(g, a, actvPlayer, vector) == TRUE){
				bol = TRUE;
			}	
		}
	}
	return bol;
}

static int adjARCs(Game g,action a, int actvPlayer, int vector[]){
	int bol = FALSE;
	int count1 = 0;
	int count2 = 0;
	int noARCs = getARCs(g, actvPlayer) ; 
	int x1 = vector [0];
	int y1 = vector [1];
	int x2 = vector [2];
	int y2 = vector [3];
	while (count1 < noARCs + 1){
		count2 = 0;	
		if (( g->map[y2][x2] == actvPlayer )||( g->map[y1][x1] == actvPlayer)){
			bol = TRUE;
		}
		while (count2 < 4 && bol != TRUE){
			if ((x1 == g->uni[actvPlayer - 1].arcs[count1][count2] \
						&& y1 == g->uni[actvPlayer - 1].arcs[count1][count2 +1]) \
					|| (x2 == g->uni[actvPlayer - 1].arcs[count1][count2] \
						&& y2 == g->uni[actvPlayer - 1].arcs[count1][count2 +1])){
			bol = TRUE;
			}
			count2 = count2 + 2;
			
		} 
	count1++;
	}
	return bol;
}

static int highestNumber(int intA, int intB, int intC) {
	int highest;
	if (intA > intB && intA > intC){
		highest = UNI_A;
	} else if (intB > intA && intB > intC){
		highest = UNI_B;
	} else if (intC > intA && intC > intB){
		highest = UNI_C;
	} else {
		highest = NO_ONE;
	}
	return highest;
}
/* 
 *this function takes in a path and an array to write in
 * and writes in the last vector in the array "lastVector"
 *
 * Logic:
 * 1. loop through each char of the path
 * 2. check the direction of the current vector
 * 3. check if next char is L, R or B
 * 4. adjust current vector to new vector
 */
static void lostArc (Game g, path path, int lastVector[]){
	//vector values for starting vector
	int x1 = 4;
	int y1 = 11;
	int x2 = 5;
	int y2 = 10;
	int ilegal = FALSE;
	int next = 0;
	while ((strncmp(&path[next], "L",1) == 0 || strncmp(&path[next], "R",1) == 0 \
			|| strncmp(&path[next], "B",1) == 0) && ilegal == FALSE){
		if (x2-x1 == 1){
			if (y2-y1 == 0){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2++; y2++;
				} else if (strncmp(&path[next], "R",1) == 0){
					x2++; y2--;
				} else {
					x2++;
				}
			} else if (y2-y1 == 1){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2--;y2++; 
				} else if (strncmp(&path[next], "R",1) == 0){
					x2++;
				} else {
					x2--;y2--;
				}
			} else if (y2-y1 == -1){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2++;
				} else if (strncmp(&path[next], "R",1) == 0){
					x2--; y2--;
				} else {
					x2--;y2++;
				}
			}
		} else {
			if (y2-y1 == 0){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2--; y2--;
				} else if (strncmp(&path[next], "R",1) == 0){
					x2--; y2++;
				} else {
					x2++;
				}
			} else if (y2-y1 == 1){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2--;
				} else if (strncmp(&path[next], "R",1) == 0){
					x2++; y2++;
				} else {
					x2++;y2--;
				}
			} else if (y2-y1 == -1){
				x1=x2; y1=y2;
				if (strncmp(&path[next], "L",1) == 0){
					x2++; y2--;
				} else if (strncmp(&path[next], "R",1) == 0){
					x2--;
				} else {
					x2++;y2++;
				}
			}
		}

		next++; 
		if (isLegalPath(g, x2, y2) == FALSE){
			x1 = -1;
			x2 = -1;
			y1 = -1;
			y2 = -1;	
			ilegal = TRUE;
		}
	}
	lastVector[0] = x1;
	lastVector[1] = y1;
	lastVector[2] = x2;
	lastVector[3] = y2;
}

static int isLegalPath(Game g, int x2, int y2){
	int bol = FALSE;
	if (x2 < 12 && x2 > -1 && y2 < 11 && y2 > -1){	
		if (g->map[y2][x2] == 9) {
			bol = FALSE;
		} else {
		bol = TRUE;
		}
	}
	return bol;
}

static void generateDisciplines(Game g, int diceScore){
	int count = 0;
	int countUni = 0;
	int countX = 0;
	int countY = 0;
	int countRegion = 0;
	int regions[NUM_REGIONS];
	int matches = 0;

	while (count < NUM_REGIONS){
		if (getDiceValue(g, count) == diceScore){
			regions[matches++] = count;
		}
	count++;
	}
	count = 0;
	while (count < matches){
		countX = 0;
		countRegion = 0;
		while (countX < HORIZONTAL_VERTICES){
			countY = 0;
			while (countY < VERTICAL_VERTICES){
				if (g->map[countY][countX] == 8 && (g->map[countY][countX + 1] == 8)){
					if (countRegion == regions[count]){
						countUni = 0;
						while (countUni < 3){		
							g->uni[countUni].students[getDiscipline(g, regions[count])] += \
							   checkAdjUni(g, countX, countY, countUni);
							countUni++;
						}
					}
					countRegion++;
				}
				countY++;
			}
			countX++;
		}
		count++;
	}
	if( diceScore == 7){
		countUni = 0;
		while (countUni < 3){
			g->uni[countUni].students[STUDENT_THD] += \
			g->uni[countUni].students[STUDENT_MTV] += \
			g->uni[countUni].students[STUDENT_MMONEY]; 
			g->uni[countUni].students[STUDENT_THD] = 0;
			g->uni[countUni].students[STUDENT_THD] = 0;
			countUni++;
		}
	}	
	return;
}
static int checkAdjUni(Game g,int x, int y, int uni){
	int result = 0;
	uni++;
	if (g->map[y-1][x] == uni || g->map[y-1][x] == uni + 3){
		result++;	
	}
	if (g->map[y+1][x] == uni || g->map[y+1][x] == uni + 3){
		result++;	
	}
	if (g->map[y-1][x+1] == uni || g->map[y-1][x+1] == uni + 3){	
		result++;	
	}
	if (g->map[y][x+2] == uni || g->map[y][x+2] == uni + 3){
		result++;	
	}
	if (g->map[y][x-1] == uni || g->map[y][x-1] == uni + 3){
		result++;	
	}
	if (g->map[y+1][x+1] == uni || g->map[y+1][x+1] == uni + 3){
		result++;	
	}
	return result;
}
/* void randomRegions(void){ */
	/* 	numbers 2-12 (random)
	 *	1 thd 0
	 *	3 bps 1 1 1
	 *	4 b?  2 2 2 2
	 *	4 mj  3 3 3 3 
	 *	4 mtv 4 4 4 4 
	 *	3 m$  5 5 5 
	 *
	 *  Describing hexes by the left side of the inside.
	 *     - storing type left, and dice number right (x+1)
	 *     - numbers in [y, x] from map-sketch
	 *  g->map[y][x]
	 *	hex1  = [3,6] //[y, x]	
	 *  hex2  = [4,4]
	 *  hex3  = [4,8]
	 *  hex4  = [5,2]
	 *  hex5  = [5,6]
	 *  hex6  = [5,10]
	 *  hex7  = [6,4]
	 *  hex8  = [6,8]
	 *  hex9  = [7,2]
	 *  hex10 = [7,6]
	 *  hex11 = [7,10]
	 *  hex12 = [8,4]
	 *  hex13 = [8,8]
	 *  hex14 = [9,2]
	 *  hex15 = [9,6]
	 *  hex16 = [9,10]
	 *  hex17 = [10,4]
	 *  hex18 = [10,8]
	 *  hex19 = [11,6]
	 *  
	 */
	
	//assigning dicevalues (2-12)(+1 to the x value)
	// in struct and in map
/*     int dice; */
/* 	int hexLeft = NUMBER_OF_HEXES + 1; */
/* 	while (hexLeft >= 0){ */
/* 		dice = rand()%11 +2; */
/* 		hexLeft--; */
/* 		 */
/* 		g->dice[NUMBER_OF_HEXES - hexLeft] = dice */
/* 		 */
/* 		if (hexLeft = 18){ */
/* 		g->map[3,7] = dice */
/* 		} else if (hexLeft = 17){ */
/* 		g->map[4,5] = rand()%11 +2; */
/* 		} else if (hexLeft = 16){ */
/* 		g->map[4,9] = rand()%11 +2; */
/* 		} else if (hexLeft = 15){ */
/* 		g->map[5,3] = rand()%11 +2; */
/* 		} else if (hexLeft = 14){ */
/* 		g->map[5,7] = rand()%11 +2; */
/* 		} else if (hexLeft = 13){ */
/* 		g->map[5,11] = rand()%11 +2; */
/* 		} else if (hexLeft = 12){ */
/* 		g->map[6,5] = rand()%11 +2; */
/* 		} else if (hexLeft = 11){ */
/* 		g->map[6,9] = rand()%11 +2; */
/* 		} else if (hexLeft = 10){ */
/* 		g->map[7,3] = rand()%11 +2; */
/* 		} else if (hexLeft = 9){ */
/* 		g->map[7,7] = rand()%11 +2; */
/* 		} else if (hexLeft = 8){ */
/* 		g->map[7,11] = rand()%11 +2; */
/* 		} else if (hexLeft = 7){ */
/* 		g->map[8,5] = rand()%11 +2; */
/* 		} else if (hexLeft = 6){ */
/* 		g->map[8,9] = rand()%11 +2; */
/* 		} else if (hexLeft = 5){ */
/* 		g->map[9,3] = rand()%11 +2; */
/* 		} else if (hexLeft = 4){ */
/* 		g->map[9,7] = rand()%11 +2; */
/* 		} else if (hexLeft = 3){ */
/* 		g->map[9,11] = rand()%11 +2; */
/* 		} else if (hexLeft = 2){ */
/* 		g->map[10,5] = rand()%11 +2; */
/* 		} else if (hexLeft = 1){ */
/* 		g->map[10,9] = rand()%11 +2; */
/* 		} else if (hexLeft = 0){ */
/* 		g->map[11,7] = rand()%11 +2; */
/* 		} */
/* 	} */
/* 	 */
/* 	//assigning disciplines */
/* 	int i = 0; */
/* 	int discLeft = NUMBER_OF_HEXES + 1; */
/* 	int hex = 0; */
/* 	int thd = 1; int bps = 3; int bq = 4; */
/* 	int mj = 4; int mtv = 4; int mc = 3; */
/* 	int discNo = 0; */
/* 	while (thd > 0 && bps > 0 && bq > 0 && \ */
/* 			mj > 0 && mtv > 0 && mc > 0){ */
/* 		 */
/* 		// what disc to assign */
/* 		rando = rand()%19;	 */
/* 		if (i=0 && thd != 0){ */
/* 			discNo = 0; */
/* 			thd--; */
/* 		} else if (i <= 1 && i >= 3 && bps != 0){ */
/* 			discNo = 1; */
/* 			bps--; */
/* 		} else if (i <= 4 && i >= 7 && bq != 0){ */
/* 			discNo = 2; */
/* 			bq--; */
/* 		} else if (i <= 8 && i >= 11 && mj != 0){ */
/* 			discNo = 3; */
/* 			mj--; */
/* 		} else if (i <= 12 && i >= 15 && mtv != 0){ */
/* 			discNo = 4; */
/* 			mtv--; */
/* 		} else if (i <= 16 && i >= 18 && mc != 0){ */
/* 			discNo = 5; */
/* 			mc--; */
/* 		} else { */
/* 			discNo = 10 */
/* 		} */
/* 		 */
/* 		// where to assign the disc */
/* 		// in struct and in map */
/* 		if (discNo != 10){ */
/* 			discLeft--; */
/* 			 */
/* 			g->discipline[NUMBER_OF_HEXES - discLeft] = discNo */
/* 			 */
/* 			if (discLeft = 18){ */
/* 				g->map[3,6] = discNo */
/* 			} else if (discLeft = 17){ */
/* 				g->map[4,4] = discNo */
/* 			} else if (discLeft = 16){ */
/* 				g->map[4,8] = discNo */
/* 			} else if (discLeft = 15){ */
/* 				g->map[5,2] = discNo */
/* 			} else if (discLeft = 14){ */
/* 				g->map[5,6] = discNo */
/* 			} else if (discLeft = 13){ */
/* 				g->map[5,10] = discNo */
/* 			} else if (discLeft = 12){ */
/* 				g->map[6,4] = discNo */
/* 			} else if (discLeft = 11){ */
/* 				g->map[6,8] = discNo */
/* 			} else if (discLeft = 10){ */
/* 				g->map[7,2] = discNo */
/* 			} else if (discLeft = 9){ */
/* 				g->map[7,6] = discNo */
/* 			} else if (discLeft = 8){ */
/* 				g->map[7,10] = discNo */
/* 			} else if (discLeft = 7){ */
/* 				g->map[8,4] = discNo */
/* 			} else if (discLeft = 6){ */
/* 				g->map[8,8] = discNo */
/* 			} else if (discLeft = 5){ */
/* 				g->map[9,2] = discNo */
/* 			} else if (discLeft = 4){ */
/* 				g->map[9,6] = discNo */
/* 			} else if (discLeft = 3){ */
/* 				g->map[9,10] = discNo */
/* 			} else if (discLeft = 2){ */
/* 				g->map[10,4] = discNo */
/* 			} else if (discLeft = 1){ */
/* 				g->map[10,8] = discNo */
/* 			} else if (discLeft = 0){ */
/* 				g->map[11,6] = discNo */
/* 			} */
/* 		} */
/* 	} */
/* } */
