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

void pageFault (int signal);												// SIGUSR1 - Page Fault
void startProcess (char* acessList);											// Start a process with an acess list to execute
void createPageTable (unsigned int** pageTable, int* shmArea_pageTable, int user_process);				// Create shared memory area and attach refence to page table
void destroyPageTable (unsigned int* pageTable, int shmArea_pageTable);							// Deattach reference to page table
void initializeUserProcess (unsigned int** pageTable, int* shmArea_pageTable, char* acessList, int user_process);	// Create a page table for process and start process
void endUserProcess (unsigned int* pageTable, int shmArea_pageTable);							// Destroy process page table
void chooseKey(key_t* key_pageTable, int processNumber);								// Generates a key to create shared memory segment

/* Main */
int main(int argc, char * argv[])
{
	// Declarations                                                                                                       
	int pid_memManager;
	int shmArea_pageTable;
	unsigned int* pageTable;
	char* acessList = "acessList1.txt";

	signal(SIGUSR1, pageFault); // Define SIGUSR1

	pid_memManager = getpid();
	
	// Declarations
	printf("Memory Manager start\n");

	initializeUserProcess (&pageTable, &shmArea_pageTable, acessList, 0);

	waitpid(-1, NULL, 0); // Wait for all childs (user processes) to end

	endUserProcess (pageTable, shmArea_pageTable);

	printf("Memory Manager end\n");

	return 0;
}

/* Signal Handlers */

void pageFault (int signal)
{
	pid_t pid_user; // User process pid that asked for page fault handle

	pid_user = waitpid(-1, NULL, WNOHANG);

	if(pid_user >= 0) // Process had page fault issue
	{
		printf("Signal %d received! Page fault on process %d!\n", signal, pid_user);
		fflush(stdout);
		// If phisical addres is on cache
		// sleep(1);
		// Else phisical addres is on HD, make swap
		// sleep(2);
	}
	else if (pid_user == -1) // Something went wrong
	{
		printf("ERROR: PID ERROR\n");
		fflush(stdout);
		exit(1);
	}
}

/* Auxiliar Functions */

void startProcess (char* acessList)	// Start new process with an acess list
{
	int execError;
	char * user_args[2];
	pid_t pid_userProcess;

	user_args[0] = (char*) malloc (strlen(acessList)*sizeof(char));
	strcpy(user_args[0], acessList);
	user_args[1] = NULL;

	pid_userProcess = fork(); // Create a user process

	if (pid_userProcess > 0) // Memory Manager
	{
		// Memory manager process
	}
	else
	{	
		if (pid_userProcess == 0) // User Process
		{
			pid_userProcess = getpid();

			execve( "./userProcess", user_args, NULL); // Execute user process
			perror("execve failed");

		}
		else // Fork failed
		{
			perror("fork failed");
			_exit(1); // Leave
	  	}
	}
}

void createPageTable (unsigned int** pageTable, int* shmArea_pageTable, int user_process)
{
	int errorControl;
	key_t key_pageTable;
	int shmSize_pageTable = (1)*sizeof(unsigned int);

	// pow(2, 16)

	printf("Creating page table!\n");

	chooseKey(&key_pageTable, user_process);

	// Page Table - Shared Memory - GET
	*shmArea_pageTable = shmget( /* key */ key_pageTable, /* size */ shmSize_pageTable, /* flags */ IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR); 
	failVerification(*shmArea_pageTable, shm_get);

	// Page Table - Shared Memory - ATTACH
	*pageTable = (unsigned int*) shmat( /* shared memory identifier */ *shmArea_pageTable, /* shared memory adress */ NULL, /* flags */ 0); 
	if(*pageTable == (unsigned int*) -1) { errorControl = -1; }
	failVerification(errorControl , shm_at); 
}

void destroyPageTable (unsigned int* pageTable, int shmArea_pageTable)
{
	int errorControl;
	printf("Destroying page table!\n");

	errorControl = shmdt( /* adress */ pageTable); // Page Table - Shared Memory - DEATTACH
	failVerification(errorControl, shm_dt);

	// Page Table - Shared Memory - REMOVE
	errorControl = shmctl( /* shared memory identifier */ shmArea_pageTable, /* command */ IPC_RMID, /* pointer to data structure */ NULL);
	failVerification(errorControl, shm_ctl);
}

void initializeUserProcess (unsigned int** pageTable, int* shmArea_pageTable, char* acessList, int user_process)
{
		createPageTable(pageTable, shmArea_pageTable, user_process);
		startProcess(acessList);	
}

void endUserProcess (unsigned int* pageTable, int shmArea_pageTable)
{
	destroyPageTable (pageTable, shmArea_pageTable);
}

void chooseKey(key_t* key_pageTable, int processNumber)
{
	*key_pageTable = 8180 + processNumber;
	printf("Page Table of process %d is associated with key %d\n", processNumber, *key_pageTable);
}


















