#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "vector.h"

// Compilation: gcc -Wall -o vector_sample1 vector_sample1.c vector.c -std=c99 [Optional Flag: -DNDEBUG to disable runtime assertions]
// Leak Detector: valgrind --leak-check=yes ./vector_sample1

// NOTE:
// Ownership semantics: Vector_Push(), vector now owns the data 
//                      Vector_Pop(), ownership of the data is passed to the user

void
Cleanup( void *DataElement )
{
	free( DataElement );
	return;
}

void
PopulateVector( struct Vector *Vector )
{
	uint8_t *Value;

	for ( size_t i = 0; i < 5; ++i )
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value ) 
		{
			puts( "Allocation error in PopulateVector...terminating\n" );
			exit( 1 );	
		}
		
		*Value = i;
		Vector_Push( Value, Vector );
	}

	return;
}

int32_t
main( int argc, char **argv ) 
{
	struct Vector *Vector;

	Vector = Vector_Initialize( sizeof (uint8_t) );

	PopulateVector( Vector );
	
	// Using Vector_Pop(), ownership is passed onto the user (me) to free the elements
	while ( !Vector_IsEmpty( Vector ) )
	{
		uint8_t *PoppedValue;

		PoppedValue = Vector_Pop( Vector );
		printf( "%u\n", *PoppedValue );

		free( PoppedValue );
		PoppedValue = NULL;   // this isn't really necessary, but good practice in case the code changes and the loop expands so we don't use after free
	} 

	Vector_Free( Vector, Cleanup ); 
	Vector = NULL;

	return 0;
}

