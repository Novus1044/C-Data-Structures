#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>

#include "vector_queue.h"

// What are the ownership semantics of the Queue??? Are they the same as a Vector???
// VectorQueue_Dequeue() logically removes the element from the Queue, so you are responsible for cleaning it up
// see the only problem is the starting index approaches the ending index but we free over the length of the array
// so we could only need to free a small fraction of the elements 

void 
Cleanup( void *DataElement ) 
{
	free( DataElement );
	return;
}

void
PopulateQueue( struct VectorQueue *Queue )
{
	uint8_t *Value;

	for ( uint8_t i = 0; i < 3; ++i )
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value )
		{
			puts( "Allocation error in PopulateQueue...terminating" );
			exit( 1 );
		}
		
		*Value = i;
		VectorQueue_Enqueue( Value, Queue );	
	}

	return;
}

int32_t
main( void )
{
	struct VectorQueue *Queue;

	Queue = VectorQueue_Initialize();

	PopulateQueue( Queue );

	while ( !VectorQueue_IsEmpty( Queue ) )
	{
		uint8_t *DequeuedElement;
	
		DequeuedElement = VectorQueue_Dequeue( Queue );
		printf( "%u\n", *DequeuedElement );

		free( DequeuedElement );
	}

	VectorQueue_Free( Queue, Cleanup );
	Queue = NULL;

	return 0;	
}

