/* Frame - Definition */

#include <string.h>		// String
#include <stdio.h>		// Input and Output
#include <stdlib.h>		// Library Definitions

typedef struct frame Frame;						// Frame structure

Frame* newFrame (unsigned int frameIndex, pid_t processNumber);		// Create new Frame 

int getFrameBitM (Frame* f);						// Get frame bitM

void setFrameBitM (Frame* f, int bitM);					// Set frame bitM

pid_t getFrameAssociatedProcess (Frame* f);				// Get process PID

void setFrameAssociatedProcess (Frame* f, pid_t processNumber);		// Set process PID

unsigned int getFrameIndex (Frame* f);					// Get frame index

void setFrameIndex (Frame* f, unsigned int frameIndex);			// Set frame index

int getFrameAccessFrequency (Frame* f);					// Get frame access frequency

void setFrameAccessFrequency (Frame* f, int number);			// Set frame access frequency

void upFrameAccessFrequency (Frame* f);					// Increase frame access frequency

void downFrameAccessFrequency (Frame* f);				// Decrease frame access frequency

