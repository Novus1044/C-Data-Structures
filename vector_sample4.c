#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vector.h"

// Compilation: gcc -Wall -o vector_sample4 vector_sample4.c vector.c -std=c99 [Optional Flag: -DNDEBUG to turn off runtime assertions]
// Leak Detector: valgrid --leak-check=yes ./vector_sample4

// NOTE:
// Ownership semantics: Vector_Push(), vector now owns the data
//                      Vector_Pop(), ownership of the data is passed to the user

struct Pixel {
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
};

void
PopulateVector( struct Vector *Vector )
{
	struct Pixel *Pixel;

	for ( size_t i = 0; i < 5; ++i )
	{
		Pixel = (struct Pixel *)malloc( sizeof *Pixel );
		if ( !Pixel ) 
		{
			puts( "Allocation error..." );
			exit( 1 );
		}
		Pixel->Red = i;
		Pixel->Green = Pixel->Red + 1;
		Pixel->Blue = Pixel->Green + 1;

		Vector_Push( Pixel, Vector );
	}
	
	return;
}


void
Cleanup( void *DataElement )
{
	free( DataElement );
	return;
}


int32_t
main( void )
{
	struct Vector *Vector;

	Vector = Vector_Initialize( sizeof (struct Pixel) );

	PopulateVector( Vector );

	while ( !Vector_IsEmpty( Vector ) )
	{
		struct Pixel *Pixel;
		
		Pixel = Vector_Pop( Vector );
		printf( "Red   = %u\n", Pixel->Red );
		printf( "Green = %u\n", Pixel->Green );
		printf( "Blue  = %u\n\n", Pixel->Blue );

		free( Pixel );
		Pixel = NULL;

	}

	Vector_Free( Vector, Cleanup );
	Vector = NULL;

	return 0;
}


