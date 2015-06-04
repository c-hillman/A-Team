
/*
 *  The magical mechanicalTurk.c
 *	
 *  presented to you by Cameron Hillman & Ludvig A. Løddesøl
 *  Written in may 2015 for the big settlers of catan-ish competition!
 *
 *	Builds a bit of everything with a bit of magic intelligence
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "Game.h"
#include "mechanicalTurk.h"

#define NUM_RETRAINABLE 5
#define MAX_OPTIONS 10

#define CAMPUS 0
#define GO8 1
#define SPINOFF 2

static int getExcessStudentType(Game g, int ID, int type);
static void whereToBuildARC (Game g, path *bestWay, path *nextARC1, \
							path *nextARC2, int ID); //which way to build -> optimal campus position
static void whereToBuildCampus (Game g, path *bestWay, int ID); //optimal campus position
static void whereToBuildGO8 (Game g, path *bestWay, int ID); //optimal GO8 position
static int vertexScore (Game g, path *p, int disc[NUM_REGIONS], int dice[NUM_REGIONS]); //given path, gives a score saying how good it is
static int sortbySize (const void * a, const void * b); //needed to sort stuff
static void trader (Game g, action *a, int ID, int type);
static int checkSurroundings(Game g, path *pa);
static int checkArcWay (Game g, path *bestWay, int ID);


action decideAction (Game g) {
	int ID = getWhoseTurn(g);
	int noMoreCampuses = FALSE;
	int noMoreGO8s = FALSE;
	action a;
	a.actionCode = PASS;

	//checks phase
	path bestWay;
	strcpy(bestWay, "");
	whereToBuildCampus (g, &bestWay, ID);
	if (strcmp(bestWay, "") == 0){
		noMoreCampuses = TRUE;
	}
	strcpy(bestWay, "");
	whereToBuildGO8(g, &bestWay, ID);
	if ((strcmp(bestWay, "") == 0) && (noMoreCampuses == TRUE)) {
		noMoreGO8s = TRUE;
	}
	strcpy(bestWay, "");
	
	// Makes arcs & campus IF we have the resources
	if ((getStudents(g, ID, STUDENT_BPS) >= 1)
		&& (getStudents(g, ID, STUDENT_BQN) >= 1)
		&& (getCampuses(g, ID) <= 10)
		&& (a.actionCode == PASS)){
		whereToBuildCampus (g, &bestWay, ID);
		path nextARC1, nextARC2;
		strcpy(nextARC1, ""); strcpy(nextARC2, "");
		whereToBuildARC(g, &bestWay, &nextARC1, &nextARC2, ID);
		if (strcmp(bestWay, "") != 0) {
			if (getARC(g, nextARC1) == 0){
				a.actionCode = OBTAIN_ARC;
				strcpy(a.destination, nextARC1);
			} else if (getARC(g, nextARC2) == 0){
				a.actionCode = OBTAIN_ARC;
				strcpy(a.destination, nextARC2);
			} else if ((getStudents(g, ID, STUDENT_MJ) >= 1) && 
					(getStudents(g, ID, STUDENT_MTV) >= 1)){
				a.actionCode = BUILD_CAMPUS;
				strcpy(a.destination, bestWay);
			}
		}
	}
	// GO8's if we have the resources
	if ((a.actionCode == PASS) &&
		(getStudents(g, ID, STUDENT_MJ) >= 2) &&
		(getStudents(g, ID, STUDENT_MMONEY) >= 3)) {
		whereToBuildGO8(g, &bestWay, ID);
		printf("Bestway: %s, status: %d\n", bestWay, getCampus(g, bestWay));
		if (strcmp(bestWay, "") != 0){
			a.actionCode = BUILD_GO8;
			strcpy(a.destination, bestWay);
			if (isLegalAction(g, a) == FALSE) {
				a.actionCode = PASS;
			}
		}
	}

	// Trades towards Campuses
	if ((getExcessStudentType(g, ID, CAMPUS) != -1) &&
		(a.actionCode == PASS) &&
		(noMoreCampuses == FALSE)) {
		
		trader(g, &a, ID, CAMPUS);
	} 
	
	// Trades towards GO8's	
	if ((noMoreCampuses == TRUE) &&
		(noMoreGO8s == FALSE) &&
		(a.actionCode == PASS) &&
		(getExcessStudentType(g, ID, GO8) != -1)){

		trader(g, &a, ID, GO8);
	}

	// Does a spinoff if we're ((1 away from being top dawg)
	// OR (noMoreCampuses && noMoreGO8s)) && have resources
	if (((getKPIpoints (g, ID) >= 140) &&(a.actionCode == PASS)) ||
		(noMoreCampuses == TRUE && noMoreGO8s == TRUE) ||
		(((getMostPublications (g) != ID) &&
		(getPublications (g, ID) + 1 >= getPublications (g, 1)) &&
		(getPublications (g, ID) + 1 >= getPublications (g, 2)) &&
		(getPublications (g, ID) + 1 >= getPublications (g, 3))) &&
		(getCampuses(g, ID) >= 6) &&
		(a.actionCode == PASS))) {
		a.actionCode = START_SPINOFF;
		if (isLegalAction(g, a) == FALSE) {
			a.actionCode = PASS;
		}
			/*if ((getStudents(g, ID, STUDENT_MJ > 0)) &&
			(getStudents(g, ID, STUDENT_MTV > 0)) &&
			(getStudents(g, ID, STUDENT_MMONEY > 0)) &&
			a.actionCode == PASS) {
				a.actionCode = START_SPINOFF;
			}*/
	}
	
	// Trades towards spinoffs
	if ((getExcessStudentType(g, ID, SPINOFF) != -1) &&
		(a.actionCode == PASS) &&
		(noMoreCampuses == TRUE) &&
		(noMoreGO8s == TRUE)){
	
		trader(g, &a, ID, SPINOFF);
	}
	
	//if actionCode is still PASS, then test ThrowDice - if it works our game would pass infinitely
	//implement if-statement underneath
	
	return a;
}

static void trader (Game g, action *a, int ID, int type){
	int costs[5] = {0};
	if (type == CAMPUS) {
		int c[5] = {1, 1, 1, 1, 0};
		memcpy(costs, c, sizeof c);
	} else if (type == GO8) {
		int c[5] = {0, 0, 2, 0, 3};
		memcpy(costs, c, sizeof c);
	} else if (type == SPINOFF) {
		int c[5] = {0, 0, 1, 1, 1};
		memcpy(costs, c, sizeof c);
	}

    a->actionCode = RETRAIN_STUDENTS;
	a->disciplineFrom = getExcessStudentType(g, ID, type);

	if (getStudents(g, ID, STUDENT_BPS) < costs[STUDENT_BPS - 1] && a->disciplineFrom != STUDENT_BPS) {
		a->disciplineTo = STUDENT_BPS;
	} else if (getStudents(g, ID, STUDENT_BQN) < costs[STUDENT_BQN - 1] && a->disciplineFrom != STUDENT_BQN) {
		a->disciplineTo = STUDENT_BQN;
	} else if (getStudents(g, ID, STUDENT_MJ) < costs[STUDENT_MJ - 1] && a->disciplineFrom != STUDENT_MJ) {
		a->disciplineTo = STUDENT_MJ;
	} else if (getStudents(g, ID, STUDENT_MTV) < costs[STUDENT_MTV - 1] && a->disciplineFrom != STUDENT_MTV) {
		a->disciplineTo = STUDENT_MTV;
	} else {
		a->disciplineTo = STUDENT_MMONEY;
	}
}

// can take in "noMoreCampuses" & "noMoreGO8s" from decideAction
// to decide which excessStudentTypes it returns like:
// if noMoreCampuses == 1, then focus is GO8, so don't trade MJ unless 6, and Money unless 7, others 3
// if noMoreGO8s == 1, then focus is spinoffs, so don't trade MJ/MTV/Money unless 5, others 3
static int getExcessStudentType(Game g, int ID, int type) {
	int result = -1;
	int amount = 0;

	int i = 0;
	int students[NUM_RETRAINABLE];
	int exchanges[NUM_RETRAINABLE];
	int modifier[NUM_RETRAINABLE];
	int excess[NUM_RETRAINABLE];

	while (i < NUM_RETRAINABLE) {
		students[i] = getStudents(g, ID, i + 1); // +1 because ThD (0) is not trainable
		exchanges[i] = getExchangeRate(g, ID, 1, i + 1); // 3rd argument is dummy value
		i++;
	}

	if (type == CAMPUS) {
		int c[NUM_RETRAINABLE] = {1, 1, 1, 1, 0};
		memcpy(modifier, c, sizeof c);
	} else if (type == GO8) {
		int c[NUM_RETRAINABLE] = {0, 0, 2, 0, 3};
		memcpy(modifier, c, sizeof c);
	} else if (type == SPINOFF) {
		int c[NUM_RETRAINABLE] = {0, 0, 1, 1, 1};
		memcpy(modifier, c, sizeof c);
	}

	i = 0;
	while (i < NUM_RETRAINABLE) {
		excess[i] = students[i] - modifier[i];
		if (excess[i] < exchanges[i]) {
			excess[i] = 0;
		}

		if (excess[i] != 0 && excess[i] > amount) {
			result = i + 1;
			amount = excess[i];
		}
		i++;
	}

	return result;
}

// uses whereToBuildCampus to decide where to build arcs towards
// the current "optimal campus"
// bestWay is where the campus should be built
static void whereToBuildARC (Game g, path *bestWay, path *nextARC1, \
							path *nextARC2, int ID){
	//int myCamps = getCampuses(g, ID);
	path pA, pB, pC, pD, pE, pF, pG, pH;
	
	if (ID == UNI_A){
		//vertex/campus values according to map (lud)
		strcpy(pA, "RRLL");			strcpy(pB, "RL");
		strcpy(pC, "LRRL"); 		strcpy(pD, "RLRL");
		strcpy(pE, "RLRLRLR"); 		strcpy(pF, "LRRLRLRLR");
		strcpy(pG, "LRRLRLR"); 		strcpy(pH, "LRLRRLRLR");
		
		//paths according to values
		if (strcmp(*bestWay, pA) == 0) {
			strcpy(*nextARC1, "RLR");
			strcpy(*nextARC2, "RLRR");
		} else if (strcmp(*bestWay, pB) == 0) {
			strcpy(*nextARC1, "R");
			strcpy(*nextARC2, "RL");
		} else if (strcmp(*bestWay, pC) == 0) {
			strcpy(*nextARC1, "RLL");
			strcpy(*nextARC2, "LRRL");
		} else if (strcmp(*bestWay, pD) == 0) {
			strcpy(*nextARC1, "RLR");
			strcpy(*nextARC2, "RLRL");
		} else if (strcmp(*bestWay, pE) == 0) {
			strcpy(*nextARC1, "RLRLRLRLL");
			strcpy(*nextARC2, "RLRLRLRL");
		} else if (strcmp(*bestWay, pF) == 0) {
			strcpy(*nextARC1, "LRRLRLRLRLR");
			strcpy(*nextARC2, "LRRLRLRLRL");
		} else if (strcmp(*bestWay, pG) == 0) {
			strcpy(*nextARC1, "LRRLRLRLR");
			strcpy(*nextARC2, "LRRLRLRL");
		} else {
			strcpy(*nextARC1, "LRRLRLRLR");
			strcpy(*nextARC2, "LRRLRLRLL");
		} 
		
	} else if (ID == UNI_B){
		strcpy(pA, "RRLLRLR");		strcpy(pB, "RRLLR");
		strcpy(pC, "RLR"); 			strcpy(pD, "RLRLR");
		strcpy(pE, "LRRLRLRL"); 	strcpy(pF, "LRLRRLRL");
		strcpy(pG, "LRRLRL"); 		strcpy(pH, "LRLRRL");
		
		if (strcmp(*bestWay, pA) == 0) {
			strcpy(*nextARC1, "RRLLRL");
			strcpy(*nextARC2, "RRLLRLR");
		} else if (strcmp(*bestWay, pB) == 0) {
			strcpy(*nextARC1, "RRLRLL");
			strcpy(*nextARC2, "RRLLRR");
		} else if (strcmp(*bestWay, pC) == 0) {
			strcpy(*nextARC1, "RRLLR");
			strcpy(*nextARC2, "RLRR");
		} else if (strcmp(*bestWay, pD) == 0) {
			strcpy(*nextARC1, "RRLLRL");
			strcpy(*nextARC2, "RLRLRR");
		} else if (strcmp(*bestWay, pE) == 0) {
			strcpy(*nextARC1, "LRLRRLRLR");
			strcpy(*nextARC2, "LRLRRLRLRR");
		} else if (strcmp(*bestWay, pF) == 0) {
			strcpy(*nextARC1, "LRLRLRRLRL");
			strcpy(*nextARC2, "LRLRRLRLL");
		} else if (strcmp(*bestWay, pG) == 0) {
			strcpy(*nextARC1, "LRLRRLRL");
			strcpy(*nextARC2, "LRRLRLL");
		} else {
			strcpy(*nextARC1, "LRLRRLRL");
			strcpy(*nextARC2, "LRLRRLR");
		}
		
	} else {
		strcpy(pA, "RLRLRLRL");		strcpy(pB, "RRLLRLRL");
		strcpy(pC, "RRLLRL"); 		strcpy(pD, "RLRLRL");
		strcpy(pE, "LRLRRLR"); 		strcpy(pF, "LRLRR");
		strcpy(pG, "LRRLR"); 		strcpy(pH, "LRR");
		
		if (strcmp(*bestWay, pA) == 0) {
			strcpy(*nextARC1, "RLRLRLRR");
			strcpy(*nextARC2, "RLRLRLRL");
		} else if (strcmp(*bestWay, pB) == 0) {
			strcpy(*nextARC1, "RRLLRLRLRR");
			strcpy(*nextARC2, "RRLLRLRLR");
		} else if (strcmp(*bestWay, pC) == 0) {
			strcpy(*nextARC1, "RRLLRLRL");
			strcpy(*nextARC2, "RRLLRLR");
		} else if (strcmp(*bestWay, pD) == 0) {
			strcpy(*nextARC1, "RLRLRLRR");
			strcpy(*nextARC2, "RLRLRLR");
		} else if (strcmp(*bestWay, pE) == 0) {
			strcpy(*nextARC1, "LRLRRL");
			strcpy(*nextARC2, "LRLRRLR");
		} else if (strcmp(*bestWay, pF) == 0) {
			strcpy(*nextARC1, "LRLRL");
			strcpy(*nextARC2, "LRLRR");
		} else if (strcmp(*bestWay, pG) == 0) {
			strcpy(*nextARC1, "LRRLL");
			strcpy(*nextARC2, "LRRLR");
		} else {
			strcpy(*nextARC1, "LRRLL");
			strcpy(*nextARC2, "LRRL");
		}
	}
}


// check out what you get from close vertexes
// considers type depending on what stage of the game
// numbers closer to 7 is better
// 2 paths > 3 >>4. Never 5 paths to build a campus
// assumes paths are NOT built unless a campus is built too
// bestWay is the vertex where the campus should be built
static void whereToBuildCampus (Game g, path *bestWay, int ID){
	int myCamps = getCampuses(g, ID);
	int myGO8s = getGO8s(g, ID);
	int pastLegal = 0;
	int n;
	int disc[NUM_REGIONS];
	int dice[NUM_REGIONS];
	int c = 0;
	
	while (c<NUM_REGIONS){
		disc[c] = getDiscipline (g, c);
		dice[c] = getDiceValue (g, c);
		c++;
	}

    path pA, pB, pC, pD, pE, pF, pG, pH;
	int sA, sB, sC, sD, sE, sF, sG, sH;
    int score[MAX_OPTIONS];
	
	// A=1, B=2.. finds which uni we are, and plays from there
	if (ID == UNI_A){
		strcpy(pA, "RRLL");			strcpy(pB, "RL");
		strcpy(pC, "LRRL"); 		strcpy(pD, "RLRL");
		strcpy(pE, "RLRLRLR"); 		strcpy(pF, "LRRLRLRLR");
		strcpy(pG, "LRRLRLR"); 		strcpy(pH, "LRLRRLRLR");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice)/2;
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice)/2;	sH = vertexScore(g, &pH, disc, dice);
		
		if ((myCamps==2 && myGO8s < 2) || (myCamps == 3 && myGO8s == 0)){
			score[0]=sB;
			score[1]=sF;

			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sB){
					pastLegal = getCampus(g, pB);
					if (pastLegal == 0 && checkSurroundings(g, &pB) == 1){
						strcpy(*bestWay, pB);
					}
				} else if (score[n] == sF){
					pastLegal = getCampus(g, pF);
					if (pastLegal == 0 && checkSurroundings(g, &pF) == 1){
						strcpy(*bestWay, pF);
					}
				}
				n++;
				i++;
			}
		} if (strcmp(*bestWay, "") == 0) {
			score[0]=sA;			score[1]=sC;			score[2]=sD;
			score[3]=sE;			score[4]=sG;			score[5]=sH;
			
			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sA){
					pastLegal = getCampus(g, pA);
					if (pastLegal == 0 && checkSurroundings(g, &pA) == 1
						&& checkArcWay(g, &pA, ID) == TRUE){
						strcpy(*bestWay, pA);
					}
				} else if (score[n] == sC){
					pastLegal = getCampus(g, pC);
					if (pastLegal == 0 && checkSurroundings(g, &pC) == 1
						&& checkArcWay(g, &pC, ID) == TRUE){
						strcpy(*bestWay, pC);
					}
				} else if (score[n] == sD){
					pastLegal = getCampus(g, pD);
					if (pastLegal == 0 && checkSurroundings(g, &pD) == 1
						&& checkArcWay(g, &pD, ID) == TRUE){
						strcpy(*bestWay, pD);
					}
				} else if (score[n] == sE){
					pastLegal = getCampus(g, pE);
					if (pastLegal == 0 && checkSurroundings(g, &pE) == 1
						&& checkArcWay(g, &pE, ID) == TRUE){
						strcpy(*bestWay, pE);
					}
				} else if (score[n] == sG){
					pastLegal = getCampus(g, pG);
					if (pastLegal == 0 && checkSurroundings(g, &pG) == 1
						&& checkArcWay(g, &pG, ID) == TRUE){
						strcpy(*bestWay, pG);
					}
				} else if (score[n] == sH){
					pastLegal = getCampus(g, pH);
					if (pastLegal == 0 && checkSurroundings(g, &pH) == 1
						&& checkArcWay(g, &pH, ID) == TRUE){
						strcpy(*bestWay, pH);
					}
				}
				n++;
				i++;
			}
		} 

	} else if (ID == UNI_B){
		strcpy(pA, "RRLLRLR");		strcpy(pB, "RRLLR");
		strcpy(pC, "RLR"); 			strcpy(pD, "RLRLR");
		strcpy(pE, "LRRLRLRL"); 	strcpy(pF, "LRLRRLRL");
		strcpy(pG, "LRRLRL"); 		strcpy(pH, "LRLRRL");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice)/2;
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice)/2;	sH = vertexScore(g, &pH, disc, dice);
		
		if ((myCamps==2 && myGO8s < 2) || (myCamps == 3 && myGO8s == 0)){
			score[0]=sB;
			score[1]=sF;

			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sB){
					pastLegal = getCampus(g, pB);
					if (pastLegal == 0 && checkSurroundings(g, &pB) == 1){
						strcpy(*bestWay, pB);
					}
				} else if (score[n] == sF){
					pastLegal = getCampus(g, pF);
					if (pastLegal == 0 && checkSurroundings(g, &pF) == 1){
						strcpy(*bestWay, pF);
					}
				}
				n++;
				i++;
			}
		} if (strcmp(*bestWay, "") == 0) {
			score[0]=sA;			score[1]=sC;			score[2]=sD;
			score[3]=sE;			score[4]=sG;			score[5]=sH;
			
			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sA){
					pastLegal = getCampus(g, pA);
					if (pastLegal == 0 && checkSurroundings(g, &pA) == 1
						&& checkArcWay(g, &pA, ID) == TRUE){
						strcpy(*bestWay, pA);
					}
				} else if (score[n] == sC){
					pastLegal = getCampus(g, pC);
					if (pastLegal == 0 && checkSurroundings(g, &pC) == 1
						&& checkArcWay(g, &pC, ID) == TRUE){
						strcpy(*bestWay, pC);
					}
				} else if (score[n] == sD){
					pastLegal = getCampus(g, pD);
					if (pastLegal == 0 && checkSurroundings(g, &pD) == 1
						&& checkArcWay(g, &pD, ID) == TRUE){
						strcpy(*bestWay, pD);
					}
				} else if (score[n] == sE){
					pastLegal = getCampus(g, pE);
					if (pastLegal == 0 && checkSurroundings(g, &pE) == 1
						&& checkArcWay(g, &pE, ID) == TRUE){
						strcpy(*bestWay, pE);
					}
				} else if (score[n] == sG){
					pastLegal = getCampus(g, pG);
					if (pastLegal == 0 && checkSurroundings(g, &pG) == 1
						&& checkArcWay(g, &pG, ID) == TRUE){
						strcpy(*bestWay, pG);
					}
				} else if (score[n] == sH){
					pastLegal = getCampus(g, pH);
					if (pastLegal == 0 && checkSurroundings(g, &pH) == 1
						&& checkArcWay(g, &pH, ID) == TRUE){
						strcpy(*bestWay, pH);
					}
				}
				n++;
				i++;
			}
		} 
	} else {
		strcpy(pA, "RLRLRLRL");		strcpy(pB, "RRLLRLRL");
		strcpy(pC, "RRLLRL"); 		strcpy(pD, "RLRLRL");
		strcpy(pE, "LRLRRLR"); 		strcpy(pF, "LRLRR");
		strcpy(pG, "LRRLR"); 		strcpy(pH, "LRR");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice)/2;
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice)/2;	sH = vertexScore(g, &pH, disc, dice);
		
		if ((myCamps==2 && myGO8s < 2) || (myCamps == 3 && myGO8s == 0)){
			score[0]=sB;
			score[1]=sF;

			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sB){
					pastLegal = getCampus(g, pB);
					if (pastLegal == 0 && checkSurroundings(g, &pB) == 1){
						strcpy(*bestWay, pB);
					}
				} else if (score[n] == sF){
					pastLegal = getCampus(g, pF);
					if (pastLegal == 0 && checkSurroundings(g, &pF) == 1){
						strcpy(*bestWay, pF);
					}
				}
				n++;
				i++;
			}
		} if (strcmp(*bestWay, "") == 0) {
			score[0]=sA;			score[1]=sC;			score[2]=sD;
			score[3]=sE;			score[4]=sG;			score[5]=sH;
			
			qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
			pastLegal = ID;
			n = 0;
			int i = 0;
			while ((pastLegal == ID) && (i < MAX_OPTIONS)) {
				if (score[n] == sA){
					pastLegal = getCampus(g, pA);
					if (pastLegal == 0 && checkSurroundings(g, &pA) == 1
						&& checkArcWay(g, &pA, ID) == TRUE){
						strcpy(*bestWay, pA);
					}
				} else if (score[n] == sC){
					pastLegal = getCampus(g, pC);
					if (pastLegal == 0 && checkSurroundings(g, &pC) == 1
						&& checkArcWay(g, &pC, ID) == TRUE){
						strcpy(*bestWay, pC);
					}
				} else if (score[n] == sD){
					pastLegal = getCampus(g, pD);
					if (pastLegal == 0 && checkSurroundings(g, &pD) == 1
						&& checkArcWay(g, &pD, ID) == TRUE){
						strcpy(*bestWay, pD);
					}
				} else if (score[n] == sE){
					pastLegal = getCampus(g, pE);
					if (pastLegal == 0 && checkSurroundings(g, &pE) == 1
					&& checkArcWay(g, &pE, ID) == TRUE){
						strcpy(*bestWay, pE);
					}
				} else if (score[n] == sG){
					pastLegal = getCampus(g, pG);
					if (pastLegal == 0 && checkSurroundings(g, &pG) == 1
						&& checkArcWay(g, &pG, ID) == TRUE){
						strcpy(*bestWay, pG);
					}
				} else if (score[n] == sH){
					pastLegal = getCampus(g, pH);
					if (pastLegal == 0 && checkSurroundings(g, &pH) == 1
						&& checkArcWay(g, &pH, ID) == TRUE){
						strcpy(*bestWay, pH);
					}
				}
				n++;
				i++;
			}
		} 
	}
}

// returns a supreme positioning for a GO8 - based on campusfunction
static void whereToBuildGO8 (Game g, path *bestWay, int ID){
	int pastLegal = 0;
	int n;

    path pA, pB, pC, pD, pE, pF, pG, pH;
	int sA, sB, sC, sD, sE, sF, sG, sH;
    int score[MAX_OPTIONS];

    int disc[NUM_REGIONS];
    int dice[NUM_REGIONS];

	int c = 0;
	
	while (c<NUM_REGIONS){
		disc[c] = getDiscipline (g, c);
		dice[c] = getDiceValue (g, c);
		c++;
	}
	
	if (ID == UNI_A){
		strcpy(pA, "RRLL");			strcpy(pB, "RL");
		strcpy(pC, "LRRL"); 		strcpy(pD, "RLRL");
		strcpy(pE, "RLRLRLR"); 		strcpy(pF, "LRRLRLRLR");
		strcpy(pG, "LRRLRLR"); 		strcpy(pH, "LRLRRLRLR");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice);
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice);	sH = vertexScore(g, &pH, disc, dice);

		score[0]=sA;		score[1]=sC;		score[2]=sD;
		score[3]=sE;		score[4]=sG;		score[5]=sH;
			
		qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
		pastLegal = 0;
		n = 0;
		int i = 0;

		printf("%s\n", *bestWay);
		while ((pastLegal == 0) && (i < MAX_OPTIONS)) {
			if (score[n] == sA && getCampus(g, pA) == ID){
				pastLegal = ID;
				strcpy(*bestWay, pA);
			} else if ((score[n] == sB) && (getCampus(g, pB) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pB);
			} else if ((score[n] == sC) && (getCampus(g, pC) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pC);
			} else if ((score[n] == sD) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pD);
			} else if ((score[n] == sE) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pE);
			} else if ((score[n] == sF) && (getCampus(g, pF) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pF);
			} else if ((score[n] == sG) && (getCampus(g, pG) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pG);
			} else if ((score[n] == sH) && (getCampus(g, pH) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pH);
			}
			n++;
			i++;
		}
		printf("%s\n", *bestWay);
	} else if (ID == UNI_B){
		strcpy(pA, "RRLLRLR");		strcpy(pB, "RRLLR");
		strcpy(pC, "RLR"); 			strcpy(pD, "RLRLR");
		strcpy(pE, "LRRLRLRL"); 	strcpy(pF, "LRLRRLRL");
		strcpy(pG, "LRRLRL"); 		strcpy(pH, "LRLRRL");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice);
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice);	sH = vertexScore(g, &pH, disc, dice);

		score[0]=sA;		score[1]=sC;		score[2]=sD;
		score[3]=sE;		score[4]=sG;		score[5]=sH;
		
		qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
		pastLegal = 0;
		n = 0;
		int i = 0;
		while ((pastLegal == 0) && (i < MAX_OPTIONS)) {
			if (score[n] == sA && getCampus(g, pA) == ID){
				pastLegal = ID;
				strcpy(*bestWay, pA);
			} else if ((score[n] == sB) && (getCampus(g, pB) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pB);
			} else if ((score[n] == sC) && (getCampus(g, pC) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pC);
			} else if ((score[n] == sD) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pD);
			} else if ((score[n] == sE) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pE);
			} else if ((score[n] == sF) && (getCampus(g, pF) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pF);
			} else if ((score[n] == sG) && (getCampus(g, pG) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pG);
			} else if ((score[n] == sH) && (getCampus(g, pH) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pH);
			}
			n++;
			i++;
		}
	} else {
		strcpy(pA, "RLRLRLRL");		strcpy(pB, "RRLLRLRL");
		strcpy(pC, "RRLLRL"); 		strcpy(pD, "RLRLRL");
		strcpy(pE, "LRLRRLR"); 		strcpy(pF, "LRLRR");
		strcpy(pG, "LRRLR"); 		strcpy(pH, "LRR");
		
		sA = vertexScore(g, &pA, disc, dice);	sB = vertexScore(g, &pB, disc, dice);
		sC = vertexScore(g, &pC, disc, dice);	sD = vertexScore(g, &pD, disc, dice);
		sE = vertexScore(g, &pE, disc, dice);	sF = vertexScore(g, &pF, disc, dice);
		sG = vertexScore(g, &pG, disc, dice);	sH = vertexScore(g, &pH, disc, dice);
		
		score[0]=sA;		score[1]=sC;		score[2]=sD;
		score[3]=sE;		score[4]=sG;		score[5]=sH;
		
		qsort(score, MAX_OPTIONS, sizeof(int), sortbySize);
		pastLegal = 0;
		n = 0;
		int i = 0;
		while ((pastLegal == 0) && (i < MAX_OPTIONS)) {
			if (score[n] == sA && getCampus(g, pA) == ID){
				pastLegal = ID;
				strcpy(*bestWay, pA);
			} else if ((score[n] == sB) && (getCampus(g, pB) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pB);
			} else if ((score[n] == sC) && (getCampus(g, pC) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pC);
			} else if ((score[n] == sD) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pD);
			} else if ((score[n] == sE) && (getCampus(g, pD) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pE);
			} else if ((score[n] == sF) && (getCampus(g, pF) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pF);
			} else if ((score[n] == sG) && (getCampus(g, pG) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pG);
			} else if ((score[n] == sH) && (getCampus(g, pH) == ID)){
				pastLegal = ID;
				strcpy(*bestWay, pH);
			}
			n++;
			i++;
		}
	}
}

// gives a score to a vertex depending on:
// 1. dice score
// 2. disciplines
// 3. what we need
static int vertexScore (Game g, path *p, int disc[NUM_REGIONS], int dice[NUM_REGIONS]){
	int ID = getWhoseTurn(g);
	int touching[3] = {0, 0, 0};
	int s1 = 0, s2 = 0, s3 = 0;
	int d[3] = {0, 0, 0};
	int score = 0;
	int k = 0;

	if (ID == UNI_A){
		if (strcmp(*p, "RRLL") == 0){
			int t[3] = {0, 3, 4}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RL") == 0) {
			int t[3] = {3, 7, 8}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRL") == 0) {
			int t[3] = {8, 12, 13}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RLRL") == 0) {
			int t[3] = {4, 8, 9}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RLRLRLR") == 0) {
			int t[3] = {5, 6, 10}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRLRLRLR") == 0) {
			int t[3] = {10, 11, 15}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRLRLR") == 0) {
			int t[3] = {9, 10, 14}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRLRRLRLR") == 0) {
			int t[3] = {14, 15, 18}; memcpy(touching, t, sizeof t);
		}
	} else if (ID == UNI_B) {
		if (strcmp(*p, "RRLLRLR") == 0) {
			int t[3] = {1, 2, 5}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RRLLR") == 0) {
			int t[3] = {0, 1, 4}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RLR") == 0) {
			int t[3] = {3, 4, 8}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RLRLR") == 0) {
			int t[3] = {4, 5, 9}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRLRLRL") == 0) {
			int t[3] = {10, 14, 15}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRLRRLRL") == 0) {
			int t[3] = {14, 17, 18}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRLRL") == 0) {
			int t[3] = {9, 13, 14}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRLRRL") == 0) {
			int t[3] = {13, 16, 17}; memcpy(touching, t, sizeof t);
		}
	} else {
		if (strcmp(*p, "RLRLRLRL") == 0) {
			int t[3] = {6, 10, 11}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RRLLRLRL") == 0) {
			int t[3] = {2, 5, 6}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RRLLRL") == 0) {
			int t[3] = {1, 4, 5}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "RLRLRL") == 0) {
			int t[3] = {5, 9, 10}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRLRRLR") == 0) {
			int t[3] = {13, 14, 17}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRLRR") == 0) {
			int t[3] = {12, 13, 16}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRRLR") == 0) {
			int t[3] = {8, 9, 13}; memcpy(touching, t, sizeof t);
		} else if (strcmp(*p, "LRR") == 0) {
			int t[3] = {7, 8, 12}; memcpy(touching, t, sizeof t);
		}
	}
	
	//locating potential THD
	if (disc[touching[0]] == 0){
		s1 = 20; s2 = dice[touching[1]]; s3 = dice[touching[2]];
	} else if (disc[touching[1]] == 0){
		s1 = dice[touching[0]]; s2 = 20; s3 = dice[touching[2]];
	} else if (disc[touching[0]] == 0){
		s1 = dice[touching[0]]; s2 = dice[touching[1]]; s3 = 20;
	} else {
		s1 = dice[touching[0]]; s2 = dice[touching[1]]; s3 = dice[touching[2]];
	}
	

	if (getCampuses(g, ID) <= 5){
		while (k < 3){
			if (disc[touching[k]] == STUDENT_THD){
				d[k] = 10;
			} else if (disc[touching[k]] == STUDENT_BPS){
				d[k] = 1;
			} else if (disc[touching[k]] == STUDENT_BQN){
				d[k] = 1;
			} else if (disc[touching[k]] == STUDENT_MJ){
				d[k] = 4;
			} else if (disc[touching[k]] == STUDENT_MTV){
				d[k] = 6;
			} else if (disc[touching[k]] == STUDENT_MMONEY){
				d[k] = 3;
			}
			k++;
		}
	} else {
		while (k < 3){
			if (disc[touching[k]] == STUDENT_THD){
				d[k] = 10;
			} else if (disc[touching[k]] == STUDENT_BPS){
				d[k] = 3;
			} else if (disc[touching[k]] == STUDENT_BQN){
				d[k] = 3;
			} else if (disc[touching[k]] == STUDENT_MJ){
				d[k] = 1;
			} else if (disc[touching[k]] == STUDENT_MTV){
				d[k] = 6;
			} else if (disc[touching[k]] == STUDENT_MMONEY){
				d[k] = 1;
			}
			k++;
		}
	}
	
	s1 = pow(s1 - 7, 2) + 5; //min = 5, max = 30
	s2 = pow(s2 - 7, 2) + 5;
	s3 = pow(s3 - 7, 2) + 5;

	int t[3] = {0, 0, 0};
	if (memcmp(touching, t, sizeof t) == 0) {
		score = rand() % 100; 
	} else {
		score = 1000 - s1*d[0] - s2*d[1] - s3*d[2]; 
		score = score - 10*d[0] - 10*d[1] - 10*d[2];
		score = score*10 + rand()%100; //some randomness to make sure no scores are the same
	}
	return score;
}

static int sortbySize (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}

static int checkSurroundings(Game g, path *pa) {
	int result = 0;

	path p;
	strcpy(p, *pa);

	int l, r, b;
	int len = strlen(p);
	
	strcat(p, "L");
	l = getCampus(g, p);
	p[len] = '\0'; strcat(p, "R");
	r = getCampus(g, p);
	p[len] = '\0'; strcat(p, "B");
	b = getCampus(g, p);
	p[len] = '\0';

	if (l == 0 && r == 0 && b == 0) {
		result = 1;
	}
;	return result;
}

//takes in BestWay from whereToBuildCampus
//Checks if the paths towards it is blocked
//Returns TRUE if available, FALSE if not.
static int checkArcWay (Game g, path *bestWay, int ID){
	path nextARC1, nextARC2;
	int answer = FALSE;
	
	whereToBuildARC(g, bestWay, &nextARC1, &nextARC2, ID);
	
	if (((getARC(g, nextARC1) == 0)||(getARC(g, nextARC1) == ID))&&
		((getARC(g, nextARC2) == 0)||(getARC(g, nextARC2) == ID))){
		answer = TRUE;
	}
	
	return answer;
}