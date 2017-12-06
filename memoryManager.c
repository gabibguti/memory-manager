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
#include "cacheList.h"		// List Definitions	

#define PAGE_TABLE_SIZE 20

enum frameValidation 	
{
	invalid		=	0,
	valid	        =	1
};

typedef enum frameValidation frameVal;			// Frame validation

List* cache;						// Global cache list

void pageFault (int signal);												// SIGUSR1 - Page Fault
void startProcess (char* accessList);											// Start a process with an access list to execute
void createPageTable (unsigned int** pageTable, int* shmArea_pageTable, int user_process);				// Create shared memory area and attach refence to page table
void destroyPageTable (unsigned int* pageTable, int shmArea_pageTable);							// Deattach reference to page table
void initializeUserProcess (unsigned int** pageTable, int* shmArea_pageTable, char* accessList, int user_process);	// Create a page table for process and start process
void endUserProcess (unsigned int* pageTable, int shmArea_pageTable);							// Destroy process page table
void chooseKey(key_t* key_pageTable, int processNumber);								// Generates a key to create shared memory segment
void intializePageTable (unsigned int* pageTable);
frameVal findFrameIndex (unsigned int* pageTable, unsigned int pageIndex, unsigned int* frameIndex);

/* Main */
int main(int argc, char * argv[])
{
	// Declarations                                                                                                       
	int pid_memManager;
	int shmArea_pageTable;
	unsigned int* pageTable;
	char* accessList = "accessList1.txt";
	Frame* f = newFrame (1, 3000);	
	Frame* a;
	
	cache = newList(cache); // Create cache

	signal(SIGUSR1, pageFault); // Define SIGUSR1

	pid_memManager = getpid();
	
	printf("Memory Manager start\n");

	/*
	initializeUserProcess (&pageTable, &shmArea_pageTable, accessList, 0);

	intializePageTable (pageTable);

	waitpid(-1, NULL, 0); // Wait for all childs (user processes) to end

	endUserProcess (pageTable, shmArea_pageTable);
	*/

	cache = listInsert(cache, f);
	cache = listInsert(cache, f);

	listPrint(cache);

	printf("---------\n");

	cache = listRemove(cache, &a);
	cache = listRemove(cache, &a);
	cache = listRemove(cache, &a);

	// Finalizations
	listPrint(cache);
	cache = listFreeAll(cache); // Cache - List - FREE

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

void startProcess (char* accessList)	// Start new process with an access list
{
	int execError;
	char * user_args[2];
	pid_t pid_userProcess;

	user_args[0] = (char*) malloc (strlen(accessList)*sizeof(char));
	strcpy(user_args[0], accessList);
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
	int shmSize_pageTable = (PAGE_TABLE_SIZE)*sizeof(unsigned int);

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

void intializePageTable (unsigned int* pageTable)
{
	int i;
	unsigned int page;

	for(i = 0; i < PAGE_TABLE_SIZE; i ++)
	{
		page = i;
		page = page << 16;
		pageTable[i] = page;
//		printf("Page table [%d] = %8x\n", i, pageTable[i]);
	}
}

void initializeUserProcess (unsigned int** pageTable, int* shmArea_pageTable, char* accessList, int user_process)
{
		createPageTable(pageTable, shmArea_pageTable, user_process);
		startProcess(accessList);	
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

frameVal findFrameIndex (unsigned int* pageTable, unsigned int pageIndex, unsigned int* frameIndex)
{
	int i = (int) pageIndex;
	*frameIndex = ((pageTable[i] << 16) >> 16);

	if(*frameIndex == 0)
	{
		return invalid;
	}

	return valid;
}

















