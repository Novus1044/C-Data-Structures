#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vector.h"

// Compilation: gcc -Wall -o vector_sample3 vector_sample3.c vector.c -std=c99 [Optional Flag -DNDEBUG to turn off runtime assertions]
// Leak Detector: valgrind --leak-check=yes ./vector_sample3

// NOTE:
// Ownership semantics: Vector_Push(), vector now owns the data
//                      Vector_Pop(), ownership of the data is passed to the user


// User defined, destructor
void
Cleanup( void *DataElement )
{
	free( DataElement ); 
	return;
}


void
FirstPush( struct Vector *Vector )
{
	uint8_t *Value;

	for ( size_t i = 0; i < 10; ++i )
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value )
		{
			puts( "Allocation error in FirstPush...terminating\n" );
			exit( 1 );
		}

		*Value = i;
		Vector_Push( Value, Vector );
	}

	return;
}

void
SecondPush( struct Vector *Vector )
{
	uint8_t *Value;

	for ( size_t i = 0; i < 3; ++i )
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value )
		{
			puts( "Allocation error in SecondPush...terminating\n" );
			exit( 1 );
		}

		*Value = i;
		Vector_Push( Value, Vector );
	}

	return;
}

int32_t
main( void )
{
	struct Vector *Vector;

	Vector = Vector_Initialize( sizeof (uint8_t) );

	FirstPush( Vector );        

	// Vector currently has 10 elements
	puts( "Last three elements in the Vector:" );
	for ( size_t i = 0; i < 3; ++i )
	{
		uint8_t *PoppedValue;
		PoppedValue = Vector_Pop( Vector );  // if you needed to store this value, you need to create a new heap allocated location and copy the value over

		printf( "%u\n", *PoppedValue );
		free( PoppedValue );
		PoppedValue = NULL;
		
	}

	// Popped three elements off and free'd them
	// Now time to push over those popped values, if we did not free these previously we would leak memory
	SecondPush( Vector );

	size_t const VectorLength = Vector_GetLength( Vector ); 

	puts( "The Vector now contains elements: " );
	for ( size_t i = 0; i < VectorLength; ++i )
	{
		printf( "%u\n", *(uint8_t *)Vector_AtIndex( i, Vector ) );	  // at index only temporarily borrows a pointer to the data, Vector is responsible for free'ing
	}

	Vector_Free( Vector, Cleanup ); 	
	Vector = NULL;

	return 0;
}
