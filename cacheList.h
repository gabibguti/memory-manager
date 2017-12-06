/* Frames List - Definition */

#include <stdio.h>	// Input and Output

#include "frame.h"	// Frame Definitions

typedef struct list List;

List* newList(List* l);							// Create new frames list

int listLength (List* l);						// Calculate length of list

List* listInsert(List* l, Frame* newFrame);				// Push frame to list

List* listRemove(List* l, Frame** removedFrame);			// Pull least accessed frame from list

List* listFreeAll(List* l);						// Free all list frames

void listPrint(List* l);						// Print list frames

List* listReaccess (List* l, unsigned int frameIndex, char action);	// Rearrange frames on list by access

void processAction(Frame* f, char action);				// Modify bit M of frame depending on action
