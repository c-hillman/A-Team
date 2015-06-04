#include "Game.h"
#include "mechanicalTurk.h"

action decideAction (Game g) {
	action a;
	int i = 0;
	
	while (i < 100){
		a.actionCode = START_SPINOFF;
		makeAction(g, a);
		i++;
	}
	
	a.actionCode = PASS;
	return a;
}