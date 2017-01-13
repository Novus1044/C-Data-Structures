#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <assert.h>

#include "vector_queue.h" 
#include "vector.h"

struct VectorQueue {
	struct Vector *VectorData;
	size_t StartingIndex;
	size_t EndingIndex;
};


struct VectorQueue *
VectorQueue_Initialize( void )
{
	struct VectorQueue *NewQueue = (struct VectorQueue *)malloc( sizeof *NewQueue );
	if ( NewQueue ) 
	{
		NewQueue->VectorData = Vector_Initialize();
		NewQueue->StartingIndex = 0;
		NewQueue->EndingIndex = 0;

		return NewQueue;
	}
	else 
	{
		puts( "Allocation error in VectorQueue_Initialize...terminating" );
		exit( 1 );
	}
}


struct VectorQueue *
VectorQueue_InitializeWithCapacity( size_t const InitialCapacity )
{
	struct VectorQueue *NewQueue = (struct VectorQueue *)malloc( sizeof *NewQueue );
	if ( NewQueue )
	{
		NewQueue->VectorData = Vector_InitializeWithCapacity( InitialCapacity );
		NewQueue->StartingIndex = 0;
		NewQueue->EndingIndex = 0;

		return NewQueue;
	}
	else
	{
		puts( "Allocation error in VectorQueue_InitializeWithCapcity...terminating" );
		exit( 1 );
	}	

}


inline __attribute__ ((always_inline)) 
uint8_t
VectorQueue_IsFull( struct VectorQueue const *Queue )
{
	return Vector_IsFull( Queue->VectorData );
}


static void
VectorQueue_RemoveDequeuedElements( struct VectorQueue *Queue )
{
        void **DequeuedElements = Vector_GetDataBufferStartingLocation( Queue->VectorData );                      // starting location for elements that have already been dequeued

        void **QueuedElements = Vector_GetDataBufferStartingLocation( Queue->VectorData ) + Queue->StartingIndex; // starting location for the currently queued elements

        size_t QueuedElementsSize = (Queue->EndingIndex - Queue->StartingIndex) * (sizeof (void *));

        memmove( DequeuedElements, QueuedElements, QueuedElementsSize );   // memmove bc these are overlapping regions in memory (in the Vector data buffer)

        return;
}


static void
VectorQueue_ResetQueueParameters( struct VectorQueue *Queue )
{
        size_t const NewQueueLength = Vector_GetLength( Queue->VectorData ) - Queue->StartingIndex;

        Vector_SetLength( NewQueueLength, Queue->VectorData );

        Queue->StartingIndex = 0;
        Queue->EndingIndex = NewQueueLength;

        return;
}


static void
VectorQueue_OverwriteDequeuedElements( struct VectorQueue *Queue )
{
	VectorQueue_RemoveDequeuedElements( Queue );
	VectorQueue_ResetQueueParameters( Queue );
	
	return;
}


void 
VectorQueue_Enqueue( void const *SourceElement, 
		     struct VectorQueue *Queue )
{
	assert( Queue != NULL && "Queue previously freed with VectorQueue_Free()" );
	
	assert( Queue->VectorData != NULL && "Queue previously freed with VectorQueue_Free(). NOTE: Queue should be set to NULL after VectorQueue_Free()" );

	if ( VectorQueue_IsFull( Queue ) && Queue->StartingIndex != 0 )
	{
		VectorQueue_OverwriteDequeuedElements( Queue );
	}

	Vector_Push( SourceElement, Queue->VectorData );  
	Queue->EndingIndex++;
	
	return;	
}


// Cases: 1. Queue has been freed, and thus the data buffer has been freed, assertion should fail
//        2. The Queue is empty so we can't dequeue, return NULL
//        3. Queue has n number of elements. We return a fresh copy of element n-1
void *
VectorQueue_Dequeue( struct VectorQueue *Queue ) 
{

	assert( Queue != NULL && "Queue previously freed with VectorQueue_Free()" );

	assert( Queue->VectorData != NULL && "Queue previously freed with VectorQueue_Free(). NOTE: Queue should be set to NULL after VectorQueue_Free()" );

	if ( VectorQueue_IsEmpty( Queue ) )
	{
		puts( "Error in VectorQueue_Dequeue...queue is empty..returning NULL" );
		return NULL;
	}
	else
	{
		void **DequeuedElement = Vector_GetDataBufferStartingLocation( Queue->VectorData ) + Queue->StartingIndex;
		
		void *DequeuedElementCopy = *DequeuedElement;

		*DequeuedElement = NULL;	
		
		Queue->StartingIndex++;    
	
		return DequeuedElementCopy;
	}
} 


inline __attribute__((always_inline))
size_t
VectorQueue_GetLength( struct VectorQueue const *Queue )
{
	return Vector_GetLength( Queue->VectorData );                   
}


inline __attribute__((always_inline))
size_t
VectorQueue_GetCapacity( struct VectorQueue const *Queue )
{
	return Vector_GetCapacity( Queue->VectorData );               
}


inline __attribute__((always_inline))
uint8_t
VectorQueue_IsEmpty( struct VectorQueue const *Queue )
{
	return Queue->StartingIndex == Queue->EndingIndex;
}


// The user is responsible for setting the Queue pointer to NULL
void
VectorQueue_Free( struct VectorQueue *Queue,
		  void(*FreeFn)(void *) )
{
	Vector_Free( Queue->VectorData, FreeFn );
	Queue->VectorData = NULL;                                       		
	free( Queue );                                                   
}

