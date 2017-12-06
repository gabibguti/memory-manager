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

void openFile (FILE** file, char* filename);						// Opens a file by a file name
void closeFile (FILE* file);								// Closes a file
void readFile (FILE* file);								// Read a file and print lines
void printFileContent (char* file_name);						// Opens a file, reads it and then close the file
void trans(pid_t pid_process, unsigned int index, unsigned int offset, char action);	// Translate virtual adress to phisical adress
unsigned int searchIndex (unsigned int index);						// Search index in page table and return page frame
void getProcessKey(key_t* key_pageTable, int processNumber);
void referencePageTable (unsigned int** pageTable, int user_process);
void unreferencePageTable (unsigned int* pageTable);

/* Main */
int main(int argc, char *argv[])
{
	char* acessList;
	unsigned int* pageTable;

	acessList = (char*) malloc (strlen(argv[0])*sizeof(char));
	strcpy(acessList, argv[0]);

	printf("\n***User Process start***\n");

	referencePageTable (&pageTable, 0);

	printf("Process acess list: %s\n", acessList);
	
	printFileContent (acessList);

	unreferencePageTable (pageTable);

	printf("\n***User Process end***\n");

	return 0;
}

/* Auxiliar Functions */

void openFile (FILE** file, char* filename)
{
	*file = fopen(filename, "r");
	if(*file == NULL)
	{
		printf("Cannot open file \n");
		exit(0);
	}
}

void closeFile (FILE* file)
{
	fclose(file);
}

void readFile (FILE* file)
{
	char rw;
	unsigned int adress, index, offset;
	pid_t pid_process;

	pid_process = getpid();

	// Read from acess list of process from file
	while(fscanf(file, "%x %c ", &adress, &rw) != -1)
	{
		index = adress >> 16;
		offset = (adress << 16) >> 16;
		trans(pid_process, index, offset, rw); // Adress translation
	}
}

void printFileContent (char* file_name)
{
	FILE* file;
	char file_path[500];

	strcpy(file_path, "./");
	strcat(file_path, file_name);

	openFile (&file, file_path);
	printf("File Content:\n\n");
	readFile (file);
	printf("\nEnd of file\n");
	closeFile (file);
}

void trans(pid_t pid_process, unsigned int index, unsigned int offset, char action) // Translate a virtual adress to a phisical adress
{
	unsigned int pageFrame;
	pid_t pid_parent;

	pid_parent = getppid(); // Memory manager pid

	// Search if virtual adress has a phisical adress in page table
	pageFrame = searchIndex (index);

	if(pageFrame != 0) // Found page frame
	{
		printf("%d\t%x%x\t%c\n", pid_process, pageFrame, offset, action);
		return;
	}
		
	// If doesent have page frame, then send signal to memory manager
	kill(pid_parent, SIGUSR1); // Warn memory manager that phisical adress is not allocated in phisical memory
}

unsigned int searchIndex (unsigned int index) // Searches for index in page table and returns the correspondent page frame number
{
	unsigned int pageFrame = 0; // Zero is reserved number for invalid page frames

	if(index != 0) // Page table has index and associated page frame
	{
		pageFrame = 1;
	}
	
	return pageFrame;
}

void getProcessKey(key_t* key_pageTable, int processNumber)
{
	*key_pageTable = 8180 + processNumber;
	printf("Got Page Table key %d of process %d\n", *key_pageTable, processNumber);
}

void referencePageTable (unsigned int** pageTable, int user_process)
{
	int errorControl;
	int shmArea_pageTable;
	key_t key_pageTable;
	int shmSize_pageTable = (1)*sizeof(unsigned int);

	// pow(2, 16)

	printf("Referencing page table!\n");

	getProcessKey(&key_pageTable, user_process);

	// Program - Shared Memory - GET
	shmArea_pageTable = shmget( /* key */ key_pageTable, /* size */ shmSize_pageTable, /* flags */ IPC_CREAT | S_IRUSR | S_IWUSR);
	failVerification(shmArea_pageTable, shm_get);
 	

	// Page Table - Shared Memory - ATTACH
	*pageTable = (unsigned int*) shmat( /* shared memory identifier */ shmArea_pageTable, /* shared memory adress */ NULL, /* flags */ 0); 
	if(*pageTable == (unsigned int*) -1) { errorControl = -1; }
	failVerification(errorControl , shm_at);
}

void unreferencePageTable (unsigned int* pageTable)
{
	int errorControl;
	printf("Unreferencing page table!\n");

	errorControl = shmdt( /* adress */ pageTable); // Page Table - Shared Memory - DEATTACH
	failVerification(errorControl, shm_dt);
}

















