#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <assert.h>

#include "hashmap3.h"

// Constraint: You cannot use the same memory location for the Value
// Something like the following will cause a double free:
// 
//   uint8_t *Value = (uint8_t *)malloc( sizeof *Value );
//  *Value = 10;
//
//  for ( size_t i = 0; i < 5; ++i ) {
//	HashMap_AddEntry( "key", Value, HashMap );
//  }
//
//  Reusing that location is not allowed since the Hashmap does not clone the Value passed into it

#define HASHMAP_SLOT_CAPACITY 3

struct HashMapEntry {
	struct KeyValuePair KeyValuePair;
	struct HashMapEntry *Parent;
	struct HashMapEntry *LeftSubtree;
	struct HashMapEntry *RightSubtree;
};

struct HashMap {
	struct HashMapEntry *Slots[HASHMAP_SLOT_CAPACITY];
};

struct HashMap *
HashMap_Initialize( void )
{
	struct HashMap *HashMap = AllocateMemoryForHashMap();

	return HashMap;
}

static struct HashMap *
AllocateMemoryForHashMap( void )
{
	struct HashMap *NewHashMap = (struct HashMap *)calloc( 1, sizeof *NewHashMap );

	if ( NewHashMap )
	{
		return NewHashMap;
	}
	else
	{
		fprintf( stderr, "AllocateMemoryForHashMap...unable to allocate the HashMap\n" );
		exit( EXIT_FAILURE );
	}
}

/*************************************************************/
void
HashMap_AddEntry( char const *Key,
		  void *Value,
		  struct HashMap *HashMap )
{
	struct HashMapEntry *HashMapEntry = ComposeNewHashMapEntry( Key, Value );
	
	size_t const HashValue = CalculateHashValueForKey( Key );

	InsertEntryIntoHashMap( HashMapEntry, HashValue, HashMap );

	return;
}

static struct HashMapEntry *
ComposeNewHashMapEntry( char const *Key,
			void *Value )
{
	struct KeyValuePair NewKeyValuePair = ComposeKeyValuePair( Key, Value );
	
	struct HashMapEntry *NewHashMapEntry = AllocateMemoryForHashMapEntry();

	EmbedKeyValuePairIntoNewEntry( NewKeyValuePair, NewHashMapEntry );

	return NewHashMapEntry;
}

static struct KeyValuePair
ComposeKeyValuePair( char const *Key,
		     void *Value )
{
	size_t const KeyLength = strlen( Key );
	
	char *KeyCopy = AllocateMemoryForKeyCopy( KeyLength + 1 );

	CopyKey( KeyCopy, Key, KeyLength );

	struct KeyValuePair NewKeyValuePair = {
		.Key = KeyCopy,
		.Value = Value
	};

	return NewKeyValuePair;
}

static char *
AllocateMemoryForKeyCopy( size_t const KeyLength )
{
	char *KeyCopyBuffer = (char *)malloc( KeyLength );

	if ( KeyCopyBuffer ) 
	{
		return KeyCopyBuffer;
	}
	else
	{
		fprintf( stderr, "AllocateMemoryForKeyCopy...Unable to allocate buffer for key copy\n" );
		exit( EXIT_FAILURE );
	}
}

static void
CopyKey( char *restrict KeyCopy, 
	 char const *restrict Key,
	 size_t const KeyLength )
{
	memcpy( KeyCopy, Key, KeyLength );
	KeyCopy[KeyLength] = '\0';

	return;
}

static struct HashMapEntry *
AllocateMemoryForHashMapEntry( void )
{
	struct HashMapEntry *NewHashMapEntry = (struct HashMapEntry *)calloc( 1, sizeof *NewHashMapEntry );

	if ( NewHashMapEntry )
	{
		return NewHashMapEntry;
	}
	else
	{
		fprintf( stderr, "AllocateMemoryForHashMapEntry...Unable to allocate memory for a new HashMapEntry\n" );
		exit( EXIT_FAILURE );
	 }
}

static void
EmbedKeyValuePairIntoNewEntry( struct KeyValuePair KeyValuePair,
			       struct HashMapEntry *HashMapEntry )
{
	HashMapEntry->KeyValuePair = KeyValuePair;
	return;
}

static size_t 
CalculateHashValueForKey( char const *Key )
{
	return Hasher( Key, strlen( Key ) );
}

// NOTE: This is so trivial it is stupid, will lead to a ton of collisions, just left in for simplicity
static size_t 
Hasher( char const *Key,
	size_t const KeyLength )
{
	size_t HashSum = 0;

	for ( size_t i = 0; i < KeyLength; ++i )
	{
		HashSum += Key[i];
	}

	return HashSum % HASHMAP_SLOT_CAPACITY;
}

static void
InsertEntryIntoHashMap( struct HashMapEntry *HashMapEntry,
			size_t const HashValue,
			struct HashMap *HashMap )
{
	if ( !IsHashMapCollision( HashValue, HashMap ) )
	{	
		InsertHashMapEntryIntoOpenSlot( HashMapEntry, &HashMap->Slots[HashValue] );
	}
	else
	{
		InsertHashMapEntryIntoCollisionChain( HashMapEntry, HashMap->Slots[HashValue] );
	}

	return;
}

static bool
IsHashMapCollision( size_t const HashValue,
		    struct HashMap *HashMap )
{
	return HashMap->Slots[HashValue] != 0;
}

static void
InsertHashMapEntryIntoOpenSlot( struct HashMapEntry *HashMapEntry,
				struct HashMapEntry **OpenHashMapSlot )
{
	*OpenHashMapSlot = HashMapEntry;
	return;
}

static void
InsertHashMapEntryIntoCollisionChain( struct HashMapEntry *HashMapEntry,
				      struct HashMapEntry *ChainEntry )
{
	InsertHashMapEntryIntoSubtree( HashMapEntry, ChainEntry );
}

static void
InsertHashMapEntryIntoSubtree( struct HashMapEntry *HashMapEntry,
			       struct HashMapEntry *TreeEntry )
{
	int8_t KeyComparison = strcmp( HashMapEntry->KeyValuePair.Key, TreeEntry->KeyValuePair.Key );
		
	if ( IsKeyADuplicate( KeyComparison ) )
	{
		ReplaceTreeEntryWithHashMapEntry( TreeEntry, HashMapEntry );
	}
	else if ( KeyComparison < 0 )
	{
		InsertHashMapEntryInLeftSubtree( HashMapEntry, TreeEntry );
	}
	else 
	{
		InsertHashMapEntryInRightSubtree( HashMapEntry, TreeEntry );
	}
	
	return;
}

static bool
IsKeyADuplicate( int8_t const KeyComparison )
{
	return KeyComparison == 0;
}

static void
ReplaceTreeEntryWithHashMapEntry( struct HashMapEntry *TreeEntry,
				  struct HashMapEntry *HashMapEntry )
{	
	HashMapEntry->Parent = TreeEntry->Parent;
	HashMapEntry->LeftSubtree = TreeEntry->LeftSubtree;
	HashMapEntry->RightSubtree = TreeEntry->RightSubtree;
	
	FreeKeyValuePair( TreeEntry );

	memcpy( TreeEntry, HashMapEntry, sizeof *TreeEntry );

	return;	
}

static void 
InsertHashMapEntryInLeftSubtree( struct HashMapEntry *HashMapEntry,
				 struct HashMapEntry *TreeEntry )
{
	if ( TreeEntry->LeftSubtree == NULL )
	{
		TreeEntry->LeftSubtree = HashMapEntry;
		HashMapEntry->Parent = TreeEntry;
		return;
	}
	else
	{
		InsertHashMapEntryIntoSubtree( HashMapEntry, TreeEntry->LeftSubtree );
	}
}

static void 
InsertHashMapEntryInRightSubtree( struct HashMapEntry *HashMapEntry,
				  struct HashMapEntry *TreeEntry )
{
	if ( TreeEntry->RightSubtree == NULL )
	{
		TreeEntry->RightSubtree = HashMapEntry;
		HashMapEntry->Parent = TreeEntry;
		return;	
	}
	else
	{
		InsertHashMapEntryIntoSubtree( HashMapEntry, TreeEntry->RightSubtree );
	}
}
/****************************************************************************/
struct KeyValuePair *
HashMap_Fetch( char const *LookupKey,
               struct HashMap *HashMap )
{
        size_t const HashValue = CalculateHashValueForKey( LookupKey );

        struct HashMapEntry *RetrievedEntry = RetrieveMatchingEntryWithKey( LookupKey, HashMap->Slots[HashValue]  );

        if ( RetrievedEntry == NULL )
        {
                return NULL;
        }

        return &(RetrievedEntry->KeyValuePair);
}

static struct HashMapEntry *
RetrieveMatchingEntryWithKey( char const *LookupKey,
                              struct HashMapEntry *HashedSlot )
{
        struct HashMapEntry *TreeEntry = HashedSlot;

        // RetrieveMatchingEntryFromTree
        for (;;)
        {
                // RetrieveFailed 
                if ( TreeEntry == NULL )
                {
                        fprintf( stderr, "Hashmap entry with key %s does not exist\n", LookupKey );
                        return NULL;
                }

                int8_t KeyComparison = strcmp( LookupKey, TreeEntry->KeyValuePair.Key ); 

                // RetrieveSuccessful 
                if ( KeyComparison == 0 )
                {
                        return TreeEntry;
                }
                // SearchInLeftSubtree
                else if ( KeyComparison < 0 )
                {
                        TreeEntry = TreeEntry->LeftSubtree;
                }
                // SearchInRightSubtree
                else
                {
                        TreeEntry = TreeEntry->RightSubtree;
                }
        }
}

/**************************************************************************/
void
HashMap_RemoveEntry( char const *LookupKey,
		     struct HashMap *HashMap )
{
	size_t const HashValue = CalculateHashValueForKey( LookupKey );

	struct HashMapEntry *HashedSlot = HashMap->Slots[HashValue];

	struct HashMapEntry *RetrievedEntry = RetrieveMatchingEntryWithKey( LookupKey, HashedSlot );

	if ( RetrievedEntry != NULL ) 
	{	
		if ( IsRetrievedEntryAHashMapSlot( RetrievedEntry, HashedSlot )
			&& IsOnlyEntryInChain( RetrievedEntry ) )
		{
			RemoveRetrievedEntryFromSlot( RetrievedEntry, &HashMap->Slots[HashValue] );
		}
		else
		{
			RemoveRetrievedEntryFromChain( RetrievedEntry ); 
		}
	}

	return;
}

static bool 
IsRetrievedEntryAHashMapSlot( struct HashMapEntry const *RetrievedEntry,
			      struct HashMapEntry const *HashedSlot )
{
	return RetrievedEntry == HashedSlot;
}

static bool
IsOnlyEntryInChain( struct HashMapEntry const *RetrievedEntry )
{
	return RetrievedEntry->LeftSubtree == NULL 
		&& RetrievedEntry->RightSubtree == NULL; 
	
}

static void 
RemoveRetrievedEntryFromSlot( struct HashMapEntry *RetrievedEntry,
			      struct HashMapEntry **HashMapSlot )
{
	FreeKeyValuePair( RetrievedEntry );
	free( RetrievedEntry );
	*HashMapSlot = 0;
	return;
}

static void
RemoveRetrievedEntryFromChain( struct HashMapEntry *RetrievedEntry )
{
	RemoveTreeEntryAndAdjustSubtrees( RetrievedEntry );
	return;	
}

static void
RemoveTreeEntryAndAdjustSubtrees( struct HashMapEntry *TreeEntry )
{
	if ( TreeEntryHasEmptySubtrees( TreeEntry ) )
	{
		RemoveCurrentTreeEntryOnly( TreeEntry );
	}
	else if ( IsSubtreeEmpty( TreeEntry->RightSubtree ) )
	{
		ReplaceCurrentTreeWithLeftSubtree( TreeEntry );
	}
	else if ( IsSubtreeEmpty( TreeEntry->LeftSubtree ) )
	{
		ReplaceCurrentTreeWithRightSubtree( TreeEntry );	
	}
	else
	{
		MoveTreeEntryLeftSubtreeUnderRightSubtree( TreeEntry );
		ReplaceCurrentTreeWithRightSubtree( TreeEntry );
	}
}

static bool
TreeEntryHasEmptySubtrees( struct HashMapEntry const *TreeEntry )
{
	return IsSubtreeEmpty( TreeEntry->LeftSubtree ) &&
	       IsSubtreeEmpty( TreeEntry->RightSubtree );
}

static bool
IsSubtreeEmpty( struct HashMapEntry const *TreeEntry )
{
	return TreeEntry == NULL;
}

static void
RemoveCurrentTreeEntryOnly( struct HashMapEntry *TreeEntry )
{
	if ( TreeEntry->Parent->LeftSubtree == TreeEntry )
	{
		TreeEntry->Parent->LeftSubtree = NULL;
		FreeKeyValuePair( TreeEntry );
		free( TreeEntry );
	}
	else if ( TreeEntry->Parent->RightSubtree == TreeEntry )
	{	
		TreeEntry->Parent->RightSubtree = NULL;
		FreeKeyValuePair( TreeEntry );
		free( TreeEntry );
	}
		
	return;	
}

static void
ReplaceCurrentTreeWithLeftSubtree( struct HashMapEntry *TreeEntry )
{
	struct HashMapEntry *LeftSubtree = TreeEntry->LeftSubtree;
	
	FreeKeyValuePair( TreeEntry );
	
	memcpy( TreeEntry, LeftSubtree, sizeof *LeftSubtree );

	free( LeftSubtree );
}

static void
ReplaceCurrentTreeWithRightSubtree( struct HashMapEntry *TreeEntry )
{
	struct HashMapEntry *RightSubtree = TreeEntry->RightSubtree;

	FreeKeyValuePair( TreeEntry );
	
	memcpy( TreeEntry, RightSubtree, sizeof *RightSubtree );
	
	free( RightSubtree );

	return;
}

static void 
MoveTreeEntryLeftSubtreeUnderRightSubtree( struct HashMapEntry *TreeEntry )
{
	struct HashMapEntry *RightSubtree = TreeEntry->RightSubtree;

	struct HashMapEntry *LeftSubtreeInsertionPoint = FindLeftmostPointOfRightSubtree( TreeEntry );

	LeftSubtreeInsertionPoint->LeftSubtree = TreeEntry->LeftSubtree;
}

static struct HashMapEntry *
FindLeftmostPointOfRightSubtree( struct HashMapEntry *TreeEntry )
{
	struct HashMapEntry *LeftmostSubtree = TreeEntry->RightSubtree;

	while ( LeftmostSubtree->LeftSubtree != NULL )
	{
		LeftmostSubtree = LeftmostSubtree->LeftSubtree;
	}

	return LeftmostSubtree;
}
/***********************************************************************/
void
HashMap_Free( struct HashMap *HashMap )
{
        for ( size_t i = 0; i < HASHMAP_SLOT_CAPACITY; ++i )
        {
                FreeChainEntries( HashMap->Slots[i] );
        }

        free( HashMap );
}

// This is a recursive InOrderTraversal which is used to free the subtrees of a slot
static void
FreeChainEntries( struct HashMapEntry *CurrentEntry )
{
        if ( CurrentEntry != NULL )
        {
                FreeChainEntries( CurrentEntry->LeftSubtree );
                FreeChainEntries( CurrentEntry->RightSubtree );
                FreeKeyValuePair( CurrentEntry );
                free( CurrentEntry );
        }

        return;
}


static void
FreeKeyValuePair( struct HashMapEntry *HashMapEntry )
{
        free( HashMapEntry->KeyValuePair.Key );
        FreeHashMapValue( HashMapEntry->KeyValuePair.Value );

        return;
}

// IMPORTANT: This function is meant to be overwritten by the user who defines a function with the same name but actually handles freeing the data
// The weak attribute makes it a weak symbol by the compiler so if another definition is found in a source file this version will be excluded
// Necessary to do this to reduce argument passing of a destructor through a vast majority of the function calls
//  Options tried: 
//      1. Give a destructor to the hashmap struct
//      2. Pass in a destructor to any function whose subfunctions required any cleanup
//  These both lead to a tangled mess so I reverted and chose to use a weak linker symbol instead to reduce the pain of implementing the destructor
__attribute__((weak))
void
FreeHashMapValue( void *Value )
{
        fprintf( stderr, "You should be redefining this function...if you see this message create a function called FreeHashMapValue and provide an implementation\n" );
        return;
}


