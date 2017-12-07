/* Frames List - Implementation */

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

#include "cacheList.h"		// List Definitions

struct list								// Frames List
{
	Frame* f;
	struct list * next;
};

List* newList(List* l)							// Create new frames list
{
	l = (List*) malloc (sizeof(List));
	l = NULL;
	return l;
}

int listLength (List* l)						// Calculate length of list
{
	int i = 0;
	List* p = l;
	for(p = l; p != NULL; p = p->next)
		i++;
	return i;	
}

List* listInsert(List* l, Frame* f)				// Push frame to list
{
	List * p = l;
	List * b;
	List* new;
	Frame* fCopy;

	new = (List*) malloc (sizeof(List));
	if(new == NULL)
	{
		printf("ERROR: MEMORY ALLOCATION\n");
		exit(1);
	}

	fCopy = newFrame(getFrameIndex(f), getFrameAssociatedProcess(f));
	if(fCopy == NULL)
	{
		printf("ERROR: MEMORY ALLOCATION\n");
		exit(1);
	}

	setFrameAccessFrequency(fCopy, getFrameAccessFrequency(f));
	setFrameBitM(fCopy, getFrameBitM(f));
	upFrameAccessFrequency(fCopy);
	new->f = fCopy;
	new->next = NULL;

	if(l == NULL)
	{
		l = new;
		return l;
	}
	
	if(getFrameAccessFrequency(new->f) >= getFrameAccessFrequency(p->f))
	{
		new->next = p;
		return new;
	}
	
	b = p;
	p = p->next;
	
	while(p != NULL)
	{
		if(getFrameAccessFrequency(new->f) < getFrameAccessFrequency(p->f))
		{
			b = p;
			p = p->next;
		}
		else if(getFrameAccessFrequency(new->f) >= getFrameAccessFrequency(p->f))
		{
			new->next = p;
			b->next = new;	 	
			return l;
		}
	}
	b->next = new;

	return l;
}

List* listRemove(List* l, Frame** removedFrame)				// Pull least accessed frame from list
{
	List* p = l;
	Frame* fCopy;

	if(p == NULL)
	{
		printf("Cache is empty!\n");
		return NULL;
	}
	
	while(p->next != NULL)
	{
		p = p->next;	
	}

	fCopy = newFrame(getFrameIndex(p->f), getFrameAssociatedProcess(p->f));
	if(fCopy == NULL)
	{
		printf("ERROR: MEMORY ALLOCATION\n");
		exit(1);
	}
	setFrameAccessFrequency(fCopy, getFrameAccessFrequency(p->f));
	setFrameBitM(fCopy, getFrameBitM(p->f));
	*removedFrame = fCopy;

	p->next = NULL;
	
	return l;
}

List* listFreeAll(List* l)						// Free all list frames
{
	List* last = NULL;
	List* p;

	if(l != NULL)
	{
		for(p = l; p->next != NULL; p = p->next)
		{
			free(last);
			last = p;
		}
	}
	printf("Freeing Processes\n");	
	return NULL;
}

void listPrint(List* l)							// Print list frames
{
	int i;
	List* p = l;
	Frame* aux;
	char** arguments;
	struct timeval t;	

	while(p != NULL)
	{
		aux = p->f;
		printf("Frame Index: %4x\n", getFrameIndex(aux));
		printf("Process PID: %d\n", getFrameAssociatedProcess(aux));
		printf("Acess Frequency: %d\n", getFrameAccessFrequency(aux));
		printf("Bit M: %d\n", getFrameBitM(aux));
		fflush(stdout);
		printf("\n");

		p = p->next;
	}
}

List* listReaccess (List* l, unsigned int frameIndex, char action)	// Rearrange frames on list by access
{
	List * p = l;
	List * n;
	List* reaccessedFrame;

	if(l == NULL)
	{
		printf("Cache is empty!\n");
		return NULL;
	}

	if(getFrameIndex(p->f) == frameIndex)
	{
		reaccessedFrame = p;
		upFrameAccessFrequency (reaccessedFrame->f);
		processAction(reaccessedFrame->f, action);
		return reaccessedFrame;	
	}
	
	while(p != NULL)
	{
		if(p->next != NULL)
		{
			n = p->next;
			if(getFrameIndex(n->f) == frameIndex)
			{
				reaccessedFrame = n;
				if(n->next != NULL)
				{	
					p->next = n->next;
				}
				else
				{
					p->next = NULL;
				}
				reaccessedFrame->next = NULL;
				processAction(reaccessedFrame->f, action);
				l = listInsert(l, reaccessedFrame->f);
				return l;
			}

			p = p->next;
		}
		else
		{
			printf("Frame is not on cache!\n");
			return NULL;
		}
	}
}

void processAction(Frame* f, char action)				// Modify bit M of frame depending on action
{
	if(action == 'W')
		setFrameBitM(f, 1);
}

