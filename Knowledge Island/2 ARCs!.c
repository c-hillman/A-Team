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
#include <string.h>

#include "Game.h"
#include "mechanicalTurk.h"

static path *randomAvailableARC(void);

action decideAction (Game g) {
	action nextAction = {OBTAIN_ARC};
	strcpy(nextAction.destination, *randomAvailableARC());

	if (isLegalAction(g, nextAction) == FALSE) {
		nextAction.actionCode = PASS;
		strcpy(nextAction.destination, ""); // not really necessary
	}

	return nextAction;
}

static path *randomAvailableARC(void) {
	path *p = (*path) malloc(sizeof (path));
	return p;
}