#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vector_queue.h"

void
PopulateQueue( struct VectorQueue *Queue )
{
	uint8_t *Value;
	
	for ( uint8_t i = 0; i < 5; ++i ) // allocate 8 bytes total
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value )
		{
			puts( "Allocation error..." );
			return;
		}

		*Value = i;
		VectorQueue_Enqueue( Value, Queue );
	}	
	
	return;
}

void
Cleanup( void *DataElement )
{
	free( DataElement );
}

int32_t
main( void )
{
	struct VectorQueue *Queue;

	Queue = VectorQueue_Initialize( sizeof (uint8_t) );
	
	PopulateQueue( Queue );

	uint8_t *PoppedValue;
	PoppedValue = VectorQueue_Dequeue( Queue );
	free( PoppedValue );
	PoppedValue = VectorQueue_Dequeue( Queue );
	free( PoppedValue );
	PoppedValue = VectorQueue_Dequeue( Queue );
	free( PoppedValue );

	VectorQueue_Free( Queue, Cleanup );

	// if you set it equal to NULL you will dereference a NULL pointer
	// maybe debug assertions everywhere, the expectation is that the pointer is never NULL
	// so if we have the debug assertions, we can remove the runtime overhead and ensure the invariants
	// we could do them for the indexing too, checkt that isn't null and that it is a valid index

	// i just don't remember how to do the debug_assert() 
	// -NDEBUG 
	
	// i think this along with the free are the last two piece of the puzzle 
}
