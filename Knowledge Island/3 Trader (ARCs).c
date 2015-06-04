/*
 *  Mr Pass.  Brain the size of a planet!
 *
 *  Proundly Created by Richard Buckland
 *  Share Freely Creative Commons SA-BY-NC 3.0. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Game.h"
#include "mechanicalTurk.h"

#define NUM_RETRAINABLE 5

static path randomAvailableARC(void);
static int getExcessStudentType(Game g, int ID);

action decideAction (Game g) {
	int ID = getWhoseTurn(g);
	action nextAction;

	if (getStudents(g, ID, STUDENT_BQN) == 0) {
		nextAction.actionCode = RETRAIN_STUDENTS;
		nextAction.disciplineFrom = getExcessStudentType(g, ID);
		nextAction.disciplineTo = STUDENT_BQN;
		return nextAction;
	} else if (getStudents(g, ID, STUDENT_BPS) == 0) {
		nextAction.actionCode = RETRAIN_STUDENTS;
		nextAction.disciplineFrom = getExcessStudentType(g, ID);
		nextAction.disciplineTo = STUDENT_BPS;
		return nextAction;
	} else {
		nextAction.actionCode = OBTAIN_ARC;
		strcpy(nextAction.destination, randomAvailableARC());
		if (isLegalAction(g, nextAction) == FALSE) {
			nextAction.actionCode = PASS;
			strcpy(nextAction.destination, ""); // not really necessary
		}
	}

	return nextAction;
}

static path randomAvailableARC(void) {
	path p = "LRL";
	return p;
}

static int getExcessStudentType(Game g, int ID) {
	int result;

	int i = 0;
	int students[NUM_RETRAINABLE];
	int exchanges[NUM_RETRAINABLE];

	while (i < NUM_RETRAINABLE) {
		students[i] = getStudents(g, ID, i + 1); // +1 because ThD (0) is not trainable
		exchanges[i] = getExchangeRate(g, ID, 1, i + 1); // 3rd argument is dummy value
	}

	i = 0;
	while (i < NUM_RETRAINABLE && i != 0 && i != 1) { // student type can't be BQN or BPS
		if (students[i] > exchanges[i]) { // if there are enough students to retrain
			result = i;
		}
	}

	// high risk student types tend to be at the end, so this method works
	// eg. MMONEY (i = 4) is high risk (7) at the end

	return result;
}