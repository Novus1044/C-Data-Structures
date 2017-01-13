#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <assert.h>

#define HASHMAP_SLOT_CAPACITY 3

struct KeyValuePair {
	char *Key;
	void *Value;
};

struct HashMapEntry;

struct HashMap;

struct HashMap *
HashMap_Initialize( void );

static struct HashMap *
AllocateMemoryForHashMap( void );

/*************************************************************/
void
HashMap_AddEntry( char const *Key,
		  void *Value,
		  struct HashMap *HashMap );

static struct HashMapEntry *
ComposeNewHashMapEntry( char const *Key,
			void *Value );

static struct KeyValuePair
ComposeKeyValuePair( char const *Key,
		     void *Value );

static char *
AllocateMemoryForKeyCopy( size_t const KeyLength );

static void
CopyKey( char *restrict KeyCopy, 
	 char const *restrict Key,
	 size_t const KeyLength );

static struct HashMapEntry *
AllocateMemoryForHashMapEntry( void );

static void
EmbedKeyValuePairIntoNewEntry( struct KeyValuePair KeyValuePair,
			       struct HashMapEntry *HashMapEntry );

static size_t 
CalculateHashValueForKey( char const *Key );

static size_t 
Hasher( char const *Key,
	size_t const KeySize );

static void
InsertEntryIntoHashMap( struct HashMapEntry *HashMapEntry,
			size_t const HashValue,
			struct HashMap *HashMap );

static bool
IsHashMapCollision( size_t const HashValue,
		    struct HashMap *HashMap );

static void
InsertHashMapEntryIntoOpenSlot( struct HashMapEntry *HashMapEntry,
				struct HashMapEntry **OpenHashMapSlot );

static void
InsertHashMapEntryIntoCollisionChain( struct HashMapEntry *HashMapEntry,
				      struct HashMapEntry *ChainEntry );

static void
InsertHashMapEntryIntoSubtree( struct HashMapEntry *HashMapEntry,
			       struct HashMapEntry *TreeEntry );

static bool
IsKeyADuplicate( int8_t const KeyComparison );

static void
ReplaceTreeEntryWithHashMapEntry( struct HashMapEntry *TreeEntry,
				  struct HashMapEntry *HashMapEntry );

static void 
InsertHashMapEntryInLeftSubtree( struct HashMapEntry *HashMapEntry,
				 struct HashMapEntry *TreeEntry );

static void 
InsertHashMapEntryInRightSubtree( struct HashMapEntry *HashMapEntry,
				  struct HashMapEntry *TreeEntry );
/****************************************************************************/
struct KeyValuePair *
HashMap_Fetch( char const *LookupKey,
	       struct HashMap *HashMap );

static struct HashMapEntry *
RetrieveMatchingEntryWithKey( char const *LookupKey,
			      struct HashMapEntry *HashedSlot);

/****************************************************************************/
void
HashMap_RemoveEntry( char const *LookupKey,
		     struct HashMap *HashMap );

static bool
IsRetrievedEntryAHashMapSlot( struct HashMapEntry const *,
			      struct HashMapEntry const * );

static bool
IsOnlyEntryInChain( struct HashMapEntry const * );

static void
RemoveRetrievedEntryFromSlot( struct HashMapEntry *, struct HashMapEntry ** );

static void
RemoveRetrievedEntryFromChain( struct HashMapEntry *RetrievedEntry );

static void
RemoveTreeEntryAndAdjustSubtrees( struct HashMapEntry *TreeEntry );

static bool
IsSubtreeEmpty( struct HashMapEntry const *TreeEntry );

static bool
TreeEntryHasEmptySubtrees( struct HashMapEntry const *TreeEntry );

static void
RemoveCurrentTreeEntryOnly( struct HashMapEntry *TreeEntry );

static void
ReplaceCurrentTreeWithLeftSubtree( struct HashMapEntry *TreeEntry );

static void
ReplaceCurrentTreeWithRightSubtree( struct HashMapEntry *TreeEntry );

static void 
MoveTreeEntryLeftSubtreeUnderRightSubtree( struct HashMapEntry *TreeEntry );

static struct HashMapEntry *
FindLeftmostPointOfRightSubtree( struct HashMapEntry *TreeEntry );
/***********************************************************************/
void
HashMap_Free( struct HashMap *HashMap );

// This is a recursive InOrderTraversal which is used to free the subtrees of a slot
static void
FreeChainEntries( struct HashMapEntry *CurrentEntry );

static void
FreeKeyValuePair( struct HashMapEntry *HashMapEntry );

// IMPORTANT: This function is meant to be overwritten by the user who defines a function with the same name but actually handles freeing the data
// The weak attribute makes it a weak symbol by the compiler so if another definition is found in a source file this version will be excluded
// Necessary to do this to reduce argument passing of a destructor through a vast majority of the function calls
//  Options tried: 
//      1. Give a destructor to the hashmap struct
//      2. Pass in a destructor to any function whose subfunctions required any cleanup
//  These both lead to a tangled mess so I reverted and chose to use a weak linker symbol instead to reduce the pain of implementing the destructor
__attribute__((weak))
void
FreeHashMapValue( void *Value );


