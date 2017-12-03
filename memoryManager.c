/* Memory Manager */

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

#include "errorControl.h"	// Error Control Definitions	

/* Main */
int main(int argc, char * argv[])
{
	// Declarations                                                                                                       
	int pid_memManager, pid_userProcess;
	char * user_args[1] = { NULL } ;

	pid_memManager = getpid();
	
	pid_userProcess = fork(); // Create a user process

	if (pid_userProcess > 0) // Memory Manager
	{
		// Declarations
		printf("\n***Memory Manager start***\n");

		waitpid(pid_userProcess, NULL, 0); // Wait for user process to end

		printf("\n***Memory Manager end***\n");

		_exit(1); // Leave
	}
	else
	{	
		if (pid_userProcess == 0)	// User Process
		{
			pid_userProcess = getpid();

			printf("\n***P1 start***\n\n");

			execve( "./userProcess", user_args, NULL); // Execute user process
			perror("execve failed");

		}
		else // Fork failed
		{
			perror("fork failed");
			_exit(1); // Leave
	  	}
	}

	return 0;
}
