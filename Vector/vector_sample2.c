#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h> // ssize_t

#include "vector.h" 

// Compilation: gcc -Wall -o vector_sample2 vector_sample2.c vector.c -std=c99 [Optional Flag: -DNDEBUG to turn off runtime assertions]
// Leak Detection: valgrind --leak-check=yes ./vector_sample2

// NOTE:
// Ownership semantics -- Vector_Push(), vector now owns the data
//                     -- Vector_Pop(), ownership of data is passed to the user

// User defined to destroy elements that belong to the Vector
void 
Cleanup( void *DataElement )
{
	free( DataElement );                    // this can be arbitrarily complex depending on what the Vector elements are pointing to
	return;
}

void 
PopulateVector( struct Vector *Vector )
{
	uint8_t *Value;
	
	for ( uint8_t i = 0; i < 5; ++i )
	{
		Value = (uint8_t *)malloc( sizeof *Value );
		if ( !Value ) 
		{
			printf( "Allocation error in PopulateVector...terminating\n" );
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

	PopulateVector( Vector );	
	
	size_t const VectorLength = Vector_GetLength( Vector );

	// print the Vector elements, backward relative to how they would be popped
	// counting up -- size_t
	puts( "Backward:" );
	for ( size_t i = 0; i < VectorLength; ++i ) 
	{
		printf( "%u\n", *(uint8_t *)Vector_AtIndex( i, Vector ) );	 // AtIndex only temporarily borrows a pointer to the data, ownership of the data belongs to the Vector
	}
	
	// print the elements as if they were being popped	
	// counting down -- ssize_t	
	puts( "Forward:" );
	for ( ssize_t i = (VectorLength - 1); i >= 0; --i )
	{
		printf( "%u\n", *(uint8_t *)Vector_AtIndex( i, Vector ) );
	}

	Vector_Free( Vector, Cleanup ); // Vector still owns the data so we need to free each element, hence passing in `GunterDestroyerOfWorlds`
	Vector = NULL;


	return 0;
} 
