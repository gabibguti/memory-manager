/* Frame - Implementation */

#include <sys/ipc.h>		// Interprocess Comunication
#include <sys/shm.h>		// Shared Memory
#include <errno.h>		// Error
#include <sys/stat.h>		// Stat Definitions
#include <string.h>		// String
#include <sys/wait.h>		// Waitpid
#include <stdio.h>		// Input and Output
#include <stdlib.h>		// Library Definitions
#include <unistd.h>		// Symbolic Constants and Types
#include <sys/types.h>		// Types Definition

#include "frame.h"		// Frame Definitions

struct frame								// Frame
{
	unsigned int index;
	pid_t process;
	int accessFrequency;
	int M;
};

Frame* newFrame (unsigned int frameIndex, pid_t processNumber)		// Create new Frame 
{
	Frame* f = (Frame*) malloc (sizeof(Frame));

	f->index = frameIndex;
	f->process = processNumber;
	f->accessFrequency = 0;
	f->M = 0;

	return f;
}

int getFrameBitM (Frame* f)						// Get frame bitM
{
	return f->M;
}

void setFrameBitM (Frame* f, int bitM)					// Set frame bitM
{
	if(bitM != 0 && bitM != 1)
	{
		printf("ERROR: BIT M SET\n");
		return;
	}
	f->M = bitM;
}

pid_t getFrameAssociatedProcess (Frame* f)				// Get process PID
{
	return f->process;
}

void setFrameAssociatedProcess (Frame* f, pid_t processNumber)		// Set process PID
{
	f->process = processNumber;
}

unsigned int getFrameIndex (Frame* f)					// Get frame index
{
	return f->index;
}

void setFrameIndex (Frame* f, unsigned int frameIndex)			// Set frame index
{
	f->index = frameIndex;
}

int getFrameAccessFrequency (Frame* f)					// Get frame access frequency
{
	return f->accessFrequency;
}

void setFrameAccessFrequency (Frame* f, int number)			// Set frame access frequency
{
	f->accessFrequency = number;
}

void upFrameAccessFrequency (Frame* f)					// Increase frame access frequency
{
	f->accessFrequency += 1;
}

void downFrameAccessFrequency (Frame* f)				// Decrease frame access frequency
{
	f->accessFrequency -= 1;
}

