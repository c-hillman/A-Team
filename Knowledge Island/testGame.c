#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Game.h"
#include "game_struct.h"

int main (int argc, char *argv[]) {
	testGetDiceValue();
	testGetMostARCs();
	testGetMostPublications();
	testGetTurnNumber();
	testWhoseTurn();
	testGetCampus();
	testGetARC();
	testIsLegalAction();
	testGetKPIpoints();
	testGetARCs();
	testGetGO8();
	testGetCampuses();
	testGetIPs();
	testGetPublications();
	testGetStudents();
	testGetExchangeRate();

	printf("All tests passed - you are awesome!\n");
	return EXIT_SUCCESS;
}

// Lucas
void testGetDiceValue(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 9); //it is now player 1's turn.
	assert(getDiceValue(0) == dice[0]); 
	assert(getDiceValue(1) == dice[1]); 
	assert(getDiceValue(2)== dice[2]);
	assert(getDiceValue(3) == dice[3]);
	assert(getDiceValue(4) == dice[4]);
	assert(getDiceValue(5) == dice[5]);
	assert(getDiceValue(6) == dice[6]);
	assert(getDiceValue(7) == dice[7]);
	assert(getDiceValue(8) == dice[8]);
	assert(getDiceValue(9) == dice[9]);
	assert(getDiceValue(10) == dice[10]);
	assert(getDiceValue(11) == dice[11]);
	assert(getDiceValue(12) == dice[12]);
	assert(getDiceValue(13) == dice[13]);
	assert(getDiceValue(14) == dice[14]);
	assert(getDiceValue(15) == dice[15]);
	assert(getDiceValue(16) == dice[16]);
	assert(getDiceValue(17) == dice[17]);
	assert(getDiceValue(18) == dice[18]);
	
	disposeGame(g);
}

void testGetMostARCs(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g,9); //it is now player 1 turn.
	assert(getMostARCs(game g) == "NO_ONE"); //no one made any arc	
	action a;
	a.actionCode = OBTAIN_ARC;
	a.destination = "R";
	makeAction(g, action a);
	assert(getMostARCs(g) == 1);

	throwDice(g,9); //it is now player 2 turn
	a.destination == "R";
	makeAction(g,action a);
	assert(getMostARCs(g) == "1");

	a.destination == "RL";
	makeAction(g,action a);//player 2 now has 2 arcs, 1 more then player 1
	assert(getMostARCs(g) == 2);

	disposeGame (game g);
}

void testGetMostPublications(void) {

	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);
	aciton a;

	throwDice(g,11); //it is now player 1 turn.
	assert(getMostPublications(g) == "NO_ONE");
	a.actionCode = OBTAIN_ARC;
	a.destinaion = "R";
	makeAction(g, a);	

	a.destination = "RL";
	makeAction(g, a);  // Builds 2 arcs to be able to build a campus  in a purple spot

	a.actionCode = BUILD_CAMPUS;
	makeAction(g, a);




	throwDice(g,11); //it is now player 2 turn.
	a.actionCode = OBTAIN_IP_PATENT;
	makeAction(g,a);
	assert(getPublications(g, UNI_B) == 1);
	assert(getMostPublications(g) == 2);

	throwDice(g, 12);// it is now player 3 turn.
	makeAction(g,a);
	assert(getPublications(g, UNI_C) == 1);
	assert(getMostPublications(g) == 2);

	throwDice(g, 12);// it is now player 1 turn and they have enough resources to obatain
	makeAction(g, a);// 2 IP patents.
	a.actionCode = OBTAIN_IP_PATENT;
	makeAction(g, a);
	makeAction(g, a);
	assert(getPublications(g, UNI_A) == 2);
	assert(getMostPublications(g) == 1);

	disposeGame (game g);
}

void testGetTurnNumber(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);
	assert(getTurnNumber(g) == -1);
	throwDice(g,9); //it is now player 1's turn.
	assert(getTurnNumber(g) == 0);

	throwDice(g,9);
	assert(getTurnNumber(g) == 1);

	throwDice(g,9);
	assert(getTurnNumber(g) == 2);

	throwDice(g,9);
	assert(getTurnNumber(g) == 3);

	throwDice(g,9);
	assert(getTurnNumber(g) == 4);

	disposeGame (game g);
}

void testWhoseTurn(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	assert(getWhoseTurn(g) == "NO_ONE");	
	throwDice(g,9); //it is now player 1's turn.

	assert(getWhoseTurn(g) == "UNI_A");	
	throwDice(g,9); //it is now player 1's turn.

	assert(getWhoseTurn(g) == "UNI_B");	
	throwDice(g,9); //it is now player 1's turn.	

	assert(getWhoseTurn(g) == "UNI_C");	
	throwDice(g,9); //it is now player 1's turn.

	assert(getWhoseTurn(g) == "UNI_A");	
	throwDice(g,9); //it is now player 1's turn.

	disposeGame (game g);
}

// Ludvig
void testGetCampus(void){
	//before game start
	assert (getCampus(g, 0) == VACANT_VERTEX)
	assert (getCampus(g, "R") == VACANT_VERTEX)
	assert (getCampus(g, "R") == VACANT_VERTEX)
	assert (getCampus(g, "B") == VACANT_VERTEX)
	assert (getCampus(g, "LRRRLR") == VACANT_VERTEX)
	assert (getCampus(g, "RRRRRRRRRRRRRRRRRRRRRR") == VACANT_VERTEX)
	
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);
	throwDice(g, 7); // it is now player 1's turn.

	//starting arcs surrounding campuses checked anti-clockwise
	assert (getCampus(g, "RLLLLLL") == 1)				//1
	assert (getCampus(g, "RRLRL") == 2)					//2
	assert (getCampus(g, "RRLRLLRLRL") == 3)			//3
	assert (getCampus(g, "RLRLRLRLRLL") == 1)			//4
	assert (getCampus(g, "LRLRLRRLRL") == 2)			//5
	assert (getCampus(g, "LRLRL") == 3)					//6
	
	//Who gets the made arc?
		//Player 1 makes an Arc
	action a;
	a.actionCode = BUILD_ARC;
	a.destination = "RL";
	makeAction(g, a);
	
	a.actionCode = BUILD_CAMPUS;
	a.destination = "RL";
	makeAction(g, a);
	
	assert(getARC(g, "RL") == 1);
	assert(getARC(g, "RL") != 2);
	assert(getARC(g, "RL") != 3);
	assert(getARC(g, "RL") != 0);

	throwDice(g, 7); // it is now player 2's turn

	//player 2 makes an Arc
	a.actionCode = BUILD_ARC;
	a.destination = "RRLRLLL";
	makeAction(g, a);

	a.actionCode = BUILD_CAMPUS;
	a.destination = "RRLRLLL";
	makeAction(g, a);
	
	assert(getARCs(g, "RRLRLLL") == 2);
	assert(getARCs(g, "RRLRLLL") != 1);
	assert(getARCs(g, "RRLRLLL") != 3);
	assert(getARCs(g, "RRLRLLL") != 0); =
}

void testGetARC(void){
	//before game start
	assert (getARC(g, 0) == VACANT_VERTEX)
	assert (getARC(g, "R") == VACANT_VERTEX)
	assert (getARC(g, "R") == VACANT_VERTEX)
	assert (getARC(g, "B") == VACANT_VERTEX)
	assert (getARC(g, "LRRRLR") == VACANT_VERTEX)
	assert (getARC(g, "RRRRRRRRRRRRRRRRRRRRRR") == VACANT_VERTEX)
	
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);
	throwDice(g, 7); // it is now player 1's turn.

	//starting arcs surrounding campuses checked anti-clockwise 
	assert (getARC(g, "R") == 1)								//1
	assert (getARC(g, "L") == VACANT_VERTEX)
	assert (getARC(g, "RR") == VACANT_VERTEX)
	assert (getARC(g, "RRLRLL") == 2)						//2
	assert (getARC(g, "RRLRLR") == VACANT_VERTEX)
	assert (getARC(g, "RRLRL") == VACANT_VERTEX)
	assert (getARC(g, "RRLRLLRLRLL") == 3)					//3
	assert (getARC(g, "RRLRLLRLRLR") == VACANT_VERTEX)
	assert (getARC(g, "RRLRLLRLRL") == VACANT_VERTEX)
	assert (getARC(g, "RLRLRLRLRLLL") == 1)					//4
	assert (getARC(g, "RLRLRLRLRLLR") == VACANT_VERTEX)
	assert (getARC(g, "RLRLRLRLRLL") == VACANT_VERTEX)
	assert (getARC(g, "LRLRLRRLRL") == 2)					//5
	assert (getARC(g, "LRLRLRRLRLR") == VACANT_VERTEX)
	assert (getARC(g, "LRLRLRRLRLL") == VACANT_VERTEX)
	assert (getARC(g, "LRLRL") == 3)							//6
	assert (getARC(g, "LRLRLR") == VACANT_VERTEX)
	assert (getARC(g, "LRLRLR") == VACANT_VERTEX)
	
	//Who gets the made arc?
	action a;
	a.actionCode = BUILD_ARC;
	a.destination = "RL";
	makeAction(g, a);
	assert(getARC(g, "RL") == 1);
	assert(getARC(g, "RL") != 2);
	assert(getARC(g, "RL") != 3);
	assert(getARC(g, "RL") != 0);

	throwDice(g, 7); // it is now player 2's turn

	a.destination = "RRLRLLL";
	makeAction(g, a);
	assert(getARCs(g, "RRLRLLL") == 2);
	assert(getARCs(g, "RRLRLLL") != 1);
	assert(getARCs(g, "RRLRLLL") != 3);
	assert(getARCs(g, "RRLRLLL") != 0);	
}

void testIsLegalAction (void){
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	// Before game - nothing is legal
	assert(isLegalAction(g, 0) == 0)
	assert(isLegalAction(g, 1) == 0)
	assert(isLegalAction(g, 2) == 0)
	assert(isLegalAction(g, 3) == 0)
	assert(isLegalAction(g, 4) == 0)
	assert(isLegalAction(g, 5) == 0)
	assert(isLegalAction(g, 6) == 0)
	assert(isLegalAction(g, 7) == 0)

	// Game started - no resources gathered
	throwDice(g, 7); // it is now player 1's turn.
	
	assert(isLegalAction(g, 0) == TRUE)
	assert(isLegalAction(g, 1) == 0)
	assert(isLegalAction(g, 2) == 0)
	assert(isLegalAction(g, 3) == 0)
	assert(isLegalAction(g, 4) == 0)
	assert(isLegalAction(g, 5) == 0)
	assert(isLegalAction(g, 6) == 0)
	assert(isLegalAction(g, 7) == 0)
	
	// Before game
		// 1. nothing is legal
	// PASS					0
		// Always allowed, except before game start
	// OBTAIN_ARC			3
		// 1. Resources 2. Connected with own ARC 
		// 3. is VACANT_ARC 4. on map
	// BUILD_CAMPUS			1
		// 1. Resources 2. connected with own ARC 
		// 3. is VACANT_VERTEX 4. only VACANT_VERTEX within 2 spaces away
	// BUILD_GO8 			2
		// 1. Resources 2. is own Campus 3. >8 GO8's
	// START_SPINOFF			4
		// 1. Resources
	// OBTAIN_PUBLICATION	5
		// 1. NEVER - Only spinoff
	// OBTAIN_IP_PATENT		6
		// 1. NEVER - Only spinoff
	// RETRAIN_STUDENTS		7
		// 1. resources - no bonuses (3->1) 
		// 2. resources - with own specific bonus (2->1)
}

void testGetKPIpoints (void){
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	//all players
	assert (getKPIpoints(g, UNI_A) == 0)
	assert (getKPIpoints(g, UNI_B) == 0)
	assert (getKPIpoints(g, UNI_C) == 0)
	
	throwDice(g, 7); // it is now player 1's turn.
	
	//all players
	assert (getKPIpoints(g, UNI_A) == 24)
	assert (getKPIpoints(g, UNI_B) == 24)
	assert (getKPIpoints(g, UNI_C) == 24)

	//+ARC including having most ARCs
	action a;
	a.actionCode = BUILD_ARC;
	a.destination = "RL";
	makeAction(g, a);
	
	assert (getKPIpoints(g, UNI_A) == 36)
	assert (getKPIpoints(g, UNI_B) == 24)
	assert (getKPIpoints(g, UNI_C) == 24)
	
	throwDice(g, 7); // it is now player 2's turn.
	a.destination = "RRLRLLL";
	makeAction(g, a);
	
	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 26)
	assert (getKPIpoints(g, UNI_C) == 24)
	
	//+CAMPUS
	a.actionCode = BUILD_CAMPUS;
	a.destination = "RRLRLLL";
	makeAction(g, a);	
	
	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 36)
	assert (getKPIpoints(g, UNI_C) == 24)
	
	//+PUBLICATION
	a.actionCode = OBTAIN_PUBLICATION
	makeAction(g, a);	

	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 46)
	assert (getKPIpoints(g, UNI_C) == 24)
	
	throwDice(g, 7); // it is now player 3's turn.
	makeAction(g, a); 
	
	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 36)
	assert (getKPIpoints(g, UNI_C) == 24)
	
	makeAction(g, a); 	// only having the most should
	makeAction(g, a); 	// give 10 points
	makeAction(g, a); 

	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 36)
	assert (getKPIpoints(g, UNI_C) == 34)

	//+IP_PATENT
	a.actionCode = OBTAIN_IP_PATENT

	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 36)
	assert (getKPIpoints(g, UNI_C) == 44)
	
	//+GO8
	throwDice(g, 7); // it is now player 1's turn.
	throwDice(g, 7); // it is now player 2's turn.
	
	a.actionCode = BUILD_GO8;
	a.destination = "RRLRLLL";
	makeAction(g, a);	
	
	assert (getKPIpoints(g, UNI_A) == 26)
	assert (getKPIpoints(g, UNI_B) == 46)
	assert (getKPIpoints(g, UNI_C) == 44)
}

void testGetARCs (void){
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7); // it is now player 1's turn.
 
	action a;
	a.actionCode = BUILD_ARC;
	a.destination = "RLR";
	makeAction(g, a);
	assert(getARCs(g, UNI_A) == 2);
 
	a.destination = "RLRRR";
	makeAction(g, a);
	assert(getARCs(g, UNI_A) == 3);
 
	throwDice(g, 7); // it is now player 2's turn
	a.destination = "RLRLRLR";
	makeAction(g, a);
	assert(getARCs(g, UNI_B) == 2);
 
	throwDice(g, 7);
	
	throwDice(g, 7); // it is now player 1's turn
	a.destination = "RLRLRLRLRLR";
	makeAction(g, a);
	assert(getARCs(g, UNI_A) == 4);
 
	assert(getARCs(g, UNI_C) == 1);


	assert(GetArcs(g, 0) == 0)
}

// Cameron
void testGetGO8(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7); // it is now player 1's turn.

	action a;
	a.actionCode = BUILD_GO8;
	a.destination = "RLR";
	makeAction(g, a);
	// build GO8 for player 1
	assert(getGO8(g, UNI_A) == 1);

	a.destination = "RLRRR";
	makeAction(g, a);
	// build another GO8 for player 1
	assert(getGO8(g, UNI_A) == 1);

	throwDice(g, 7); // it is now player 2's turn
	a.destination = "RLRLRLR";
	makeAction(g, a);
	assert(getGO8(g, UNI_B) == 1);

	throwDice(g, 7);
	
	throwDice(g, 7); // it is now player 1's turn
	a.destination = "RLRLRLRLRLR";
	makeAction(g, a);
	assert(getGO8(g, UNI_A) == 2);

	assert(getGO8(g, UNI_C) == 0);
	disposeGame(g);
}

void testGetCampuses(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7); // it is now player 1's turn.

	action a;
	a.actionCode = BUILD_CAMPUS;
	a.destination = "RLR";
	makeAction(g, a);
	assert(getCampuses(g, UNI_A) == 1);

	a.destination = "RLRRR";
	makeAction(g, a);
	assert(getCampuses(g, UNI_A) == 1);

	throwDice(g, 7); // it is now player 2's turn
	a.destination = "RLRLRLR";
	makeAction(g, a);
	assert(getCampuses(g, UNI_B) == 1);

	throwDice(g, 7);
	
	throwDice(g, 7); // it is now player 1's turn
	a.destination = "RLRLRLRLRLR";
	makeAction(g, a);
	assert(getCampuses(g, UNI_A) == 2);

	assert(getCampuses(g, UNI_C) == 0);
	disposeGame(g);
}

void testGetIPs(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7); // it is now player 1's turn.

	action a;
	a.actionCode = OBTAIN_IP_PATENT;
	makeAction(g, a);
	assert(getIPs(g, UNI_A) == 1);

	makeAction(g, a);
	assert(getIPs(g, UNI_A) == 1);

	throwDice(g, 7); // it is now player 2's turn
	makeAction(g, a);
	assert(getIPs(g, UNI_B) == 1);

	throwDice(g, 7);
	
	throwDice(g, 7); // it is now player 1's turn
	makeAction(g, a);
	assert(getIPs(g, UNI_A) == 2);

	assert(getIPs(g, UNI_C) == 0);
	disposeGame(g);
}

void testGetPublications(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7); // it is now player 1's turn.

	action a;
	a.actionCode = OBTAIN_PUBLICATION;
	makeAction(g, a);
	assert(getPublications(g, UNI_A) == 1);

	makeAction(g, a);
	assert(getPublications(g, UNI_A) == 1);

	throwDice(g, 7); // it is now player 2's turn
	makeAction(g, a);
	assert(getPublications(g, UNI_B) == 1);

	throwDice(g, 7);
	
	throwDice(g, 7); // it is now player 1's turn
	makeAction(g, a);
	assert(getPublications(g, UNI_A) == 2);

	assert(getPublications(g, UNI_C) == 0);	
	disposeGame(g);
}

void testGetStudents(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 11); // rolling an 11 gives player 1 an MTV
	assert(getStudents(g, UNI_A, STUDENT_MTV) == 1);

	action a;
	a.actionCode = BUILD_GO8;
	a.destination = "";
	makeAction(g, a);

	throwDice(g, 11); // rolling an 11 now gives player 1 two MTVs
	assert(getStudents(g, UNI_A, STUDENT_MTV) == 3);

	throwDice(g, 8);
	assert(getStudents(g, UNI_C, STUDENT_MTV) == 1);
	assert(getStudents(g, UNI_A, STUDENT_MJ) == 1);
	disposeGame(g);
}

void testGetExchangeRate(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(disciplines, dice);

	throwDice(g, 7);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ) == 3);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ)
		== getExchangeRate(g, UNI_A, STUDNET_MTV, STUDENT_BQN));

	action a;
	a.actionCode = BUILD_CAMPUS;
	a.destination = "RR";
	makeAction(g, a);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ) == 2);
	disposeGame(g);
}