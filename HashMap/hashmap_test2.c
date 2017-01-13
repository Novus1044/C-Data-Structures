#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <stdbool.h>

#include "hashmap3.h"

// I want code to read from top to bottom
// I also want to limit scopes of variables to only their longest duration, so you could possibly block certain things off
// Functions that should be call from multiple places should have the const attribute attached to them, that only makes sense
// They are independent of the state and only have value on their return value 

// Comment otherwise -- the comments become out of sync but we don't end up with the in-depth parameter passing 
// pure and const routines kev 

void 
FreeHashMapValue( void *Data )
{
	free( Data );
	return;
}

int32_t
main( int argc, char *argv[] )
{

	char *HashedStrings[] = {
		"kevin",
		"christopher",
		"gregory",
		"michael",
		"bradley",
		"matthew",
		"james",
		"thomas",
	};

	char *DummyStrings[] = {
		"see_no_evil",
		"hear_no_evil",
		"speak_no_evil",
	};

        struct HashMap *HashMap = HashMap_Initialize( );

	// This caused the problem,
        // uint8_t *Value = (uint8_t *)malloc( sizeof *Value );
        // *Value = 10;

	for ( size_t i = 0; i < (sizeof HashedStrings)/(sizeof HashedStrings[0]); ++i )
	{
		uint8_t *Value = (uint8_t *)malloc( sizeof *Value );
		*Value = i;
		HashMap_AddEntry( HashedStrings[i], Value, HashMap );
	}
	
        struct KeyValuePair *Data; //Need to check the return value

	for ( size_t i = 0; i < (sizeof HashedStrings)/(sizeof HashedStrings[0]); ++i )
	{
		Data = HashMap_Fetch( HashedStrings[i], HashMap );
		if ( Data ) 
		{
			printf( "%s, %u\n", Data->Key, *(uint8_t *)Data->Value );
		}
	}

	for ( size_t i = 0; i < (sizeof DummyStrings)/(sizeof DummyStrings[0]); ++i )
	{
		Data = HashMap_Fetch( DummyStrings[i], HashMap );
		if ( Data )
		{
			printf( "%s, %u\n", Data->Key, *(uint8_t *)Data->Value );
		}
	}

	HashMap_RemoveEntry( "matthew", HashMap );
	HashMap_RemoveEntry( "bradley", HashMap );
	HashMap_RemoveEntry( "gregory", HashMap );
	HashMap_RemoveEntry( "christopher", HashMap );

	HashMap_Free( HashMap );



        return 0;

}

