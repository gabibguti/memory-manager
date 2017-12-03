/* User Process */

/* Includes */

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
#include <sys/time.h>		// Time Definitions

#include "errorControl.h"	// Error Control Definitions	

/* Main */
int main(int argc, char *argv[])
{
	printf("\n***User Process start***\n");
	printf("\n***User Process end***\n");

	return 0;
}
