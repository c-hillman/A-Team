#define PASS				0
#define BUILD_CAMPUS		1
#define BUILD_GO8			2
#define OBTAIN_ARC			3
#define START_SPINOFF		4
#define OBTAIN_PUBLICATION	5
#define OBTAIN_IP_PATENT	6
#define RETRAIN_STUDENTS	7

#define NO_ONE 0
#define UNI_A 1
#define UNI_B 2
#define UNI_C 3

#define DEFAULT_DISCIPLINES {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ,
							STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV,
							STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN,
							STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ,
							STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS}
#define DEFAULT_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5}

#define FALSE 0
#define TRUE  1

void testGetGO8(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

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
}

void testGetCampuses(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

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
}

void testGetIPs(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

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
}

void testGetPublications(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

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
}

void testGetStudents(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

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
}

void testGetExchangeRate(void) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE;
	game g = newGame(discipline, dice);

	throwDice(g, 7);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ) == 3);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ)
		== getExchangeRate(g, UNI_A, STUDNET_MTV, STUDENT_BQN));

	action a;
	a.actionCode = BUILD_CAMPUS;
	a.destination = "RR";
	makeAction(g, a);
	assert(getExchangeRate(g, UNI_A, STUDENT_MTV, STUDENT_MJ) == 2);
}