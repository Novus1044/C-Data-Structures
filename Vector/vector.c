#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <string.h>

#include "vector.h"  // forward declarations


struct Vector {
    void **DataBuffer;    // vector elements are pointers to heap allocated data
    size_t Length;        // the current number of elements within the vector
    size_t Capacity;      // the total capacity of the underlying data buffer, vector is full when length == capacity
    size_t DataSize;      // the size of the data pointed to by the vector element, used when we pop and create a copy of the data
};


static void **
Vector_AllocateInitialDataBuffer( size_t const InitialVectorCapacity )
{
        void **NewDataBuffer;

        NewDataBuffer = (void **)calloc( InitialVectorCapacity, sizeof (void *) );
        if ( NewDataBuffer )
        {
                return NewDataBuffer;
        }
        else
        {
                puts( "Vector_AllocateInitialDataBuffer failed to allocate initial vector buffer...exiting" );
                exit( 1 );
        }
}


struct Vector *
Vector_Initialize( size_t const ElementSize )
{
	assert( ElementSize != 0 && "Unable to create a Vector containing data with size zero" );

	struct Vector *NewVector;

	NewVector = (struct Vector *)malloc( sizeof *NewVector );
	if ( NewVector )
	{
		NewVector->Capacity = 2;            // default buffer capacity
		NewVector->Length = 0;
		NewVector->DataBuffer = Vector_AllocateInitialDataBuffer( NewVector->Capacity );
		NewVector->DataSize = ElementSize;
	
		return NewVector;
	}
	else 
	{
		puts( "Vector_Initialize failed to allocate the Vector object...exiting" );
		exit( 1 );
	}
}  


struct Vector *
Vector_InitializeWithCapacity( size_t const ElementSize,
          		       size_t const InitialCapacity  )
{
	assert( ElementSize != 0 && "Unable to create a Vector containing data with size zero" );

	struct Vector *NewVector;

	NewVector = (struct Vector *)malloc( sizeof *NewVector );
	if ( NewVector )
	{
		NewVector->Capacity = InitialCapacity;
		NewVector->Length = 0;
		NewVector->DataBuffer = Vector_AllocateInitialDataBuffer( NewVector->Capacity );
		NewVector->DataSize = ElementSize;

		return NewVector;	
	}
	else 
	{
		puts( "Vector_InitializeWithCapacity failed to allocate the Vector object...exiting" );	
		exit( 1 );
	}
}


static void
Vector_IncreaseCapacity( struct Vector *Vector )
{
	Vector->Capacity *= 2;	
	return;
}


static void
Vector_CopyCurrentBufferToNewBuffer( void **restrict NewBuffer, 
				     void const **restrict OldBuffer,
				     size_t OldBufferSize )
{
	memcpy( NewBuffer, OldBuffer, OldBufferSize ); // non-overlapping domains, memcpy
	return;
}



static void **
Vector_ResizeDataBuffer( size_t const NewVectorCapacity )
{
	void **NewBuffer;

	NewBuffer = (void **)calloc( NewVectorCapacity, sizeof (void *) );  
	if ( NewBuffer ) 
	{
		return NewBuffer;
	}
	else
	{
		puts( "Vector_ResizeDataBuffer failed to resize vector buffer...exiting" );
		exit( 1 );
	}
}

static void
Vector_FreeEntireDataBuffer( void ** const VectorDataBuffer )
{
        free( VectorDataBuffer );
        return;
}


static void
Vector_ReallocateDataBuffer( struct Vector *Vector ) 
{
	size_t OldDataBufferSize;
	void **NewDataBuffer;

	OldDataBufferSize = Vector->Capacity * sizeof (void *); // the Vector holds pointers so each element is the size of a pointer depending on architecture

	Vector_IncreaseCapacity( Vector );

	NewDataBuffer = Vector_ResizeDataBuffer( Vector->Capacity );

	Vector_CopyCurrentBufferToNewBuffer( (void **restrict)NewDataBuffer, (void const **restrict)Vector->DataBuffer, OldDataBufferSize );
	Vector_FreeEntireDataBuffer( Vector->DataBuffer );

	Vector->DataBuffer = NewDataBuffer;	
}


inline __attribute__ ((always_inline)) 
uint8_t Vector_IsFull( struct Vector const *Vector )
{
	return Vector->Length == Vector->Capacity;
}


// Cases: 
//   1. Vector could have already been deallocated so the assertions guard against use after free. The user should set the Vector pointer to NULL but the second assert is used as a safeguard
//      since internally Vector_Free() sets Vector->DataBuffer to NULL.
//   2. Vector could be empty (length == 0) so popping is again an invalid operation
//   3. Vector contains valid elements so we remove the last one by creating a copy of the data pointed to by the vector, free the vector element, then decrease the Vector's length by one. 
void *
Vector_Pop( struct Vector *Vector )
{
	assert( Vector != NULL && "Vector_Free() previously called...vector already freed" ); 

	assert( Vector->DataBuffer != NULL && "Vector_Free() previously called...vector already freed. NOTE: struct Vector * should be set to NULL folllowing Vector_Free()" );
	
	if ( Vector_IsEmpty( Vector ) )
	{
		printf( "Vector is empty...returning NULL\n" );
		return NULL;
	}
	else 
	{
		void **VectorElementLocation;
		void *VectorElementCopy;

		VectorElementLocation = Vector->DataBuffer  + (Vector->Length - 1); // after push, Vector->Length is 1 greater than the current element, so back up 1 to last element

		VectorElementCopy = (void *)malloc( Vector->DataSize );  // create a new pointer to heap allocated memory
		if ( !VectorElementCopy )
		{
			puts( "Unable to return a copy of the vector element...returning NULL" );
			return NULL;  // could just let this fall through to the return VectorElementCopy since it's value is NULL anyways
		}

		memcpy( VectorElementCopy, *VectorElementLocation, Vector->DataSize );

		free( *VectorElementLocation );
		*VectorElementLocation = NULL;

		--Vector->Length;

		return VectorElementCopy;	
	}
}


inline __attribute__((always_inline))
uint8_t Vector_IsEmpty( struct Vector const *Vector )
{
	return Vector->Length == 0;
}

/* Vector_Push needs to handle 3 cases:
 *  1. Assertion handles the case if the user has already called Vector_Free(). The user should set the pointer to the Vector to NULL but internally the free'ing function
 *     will set the Vector->DataBuffer member to NULL on a call to Vector_Free(). The second assertion catches a use after free if the user forgets to set the Vector to NULL.
 *  2. The Vector is full, we need to reallocate the internal data buffer, copy the old data to the new buffer then recursively push the data onto the new buffer
 *  3. Otherwise we need to find the next push location then copy the data into the buffer
 */
void 
Vector_Push( void const *SourceElement, 
	     struct Vector *Vector )
{
	assert( Vector != NULL && "Vector_Free() previously called...vector already freed" );
	
	assert( Vector->DataBuffer != NULL && "Vector_Free() previously called..vector already freed. NOTE: struct Vector * should be set to NULL following Vector_Free()" );

	if ( Vector_IsFull( Vector ) )
	{
		Vector_ReallocateDataBuffer( Vector );
		Vector_Push( SourceElement, Vector );	
	}
	else
	{
		void **PushLocation;
	
		PushLocation = Vector->DataBuffer + Vector->Length;   
		*PushLocation = SourceElement; // removes the const qualifer here	

		++Vector->Length;	
	}
	
	return;
}


void *
Vector_AtIndex( size_t const Index,
                struct Vector const *Vector )
{
	assert( Vector != NULL && "Vector_Free() previously called..vector already freed" );
	
	assert( Vector->DataBuffer != NULL && "Vector_Free() previously called...vector already freed. NOTE: struct Vector * should be set to NULL following Vector_Free()" );

	assert( Index < Vector->Length && "Out of bounds index of Vector" );
	
	return Vector->DataBuffer[Index];
	
}


// If an element is popped off the vector with Vector_Pop() the old location's element is freed and set to NULL.
// That requires us to check that the element isn't NULL before calling free on the location
static void
Vector_FreeAllElements( struct Vector *Vector,
                        void(*FreeFn)(void *) )
{
        for ( size_t i = 0; i < Vector->Capacity; ++i )
        {
                if ( Vector->DataBuffer[i] )
                {
                        FreeFn( Vector->DataBuffer[i] );

                        Vector->DataBuffer[i] = NULL;
                }
        }
        return;
}


void
Vector_Free( struct Vector *Vector,    
             void(*FreeFn)(void *) )
{
	Vector_FreeAllElements( Vector, FreeFn );
	Vector_FreeEntireDataBuffer( Vector->DataBuffer );
	Vector->DataBuffer = NULL;
	free( Vector );                      

	return;
}


// NOTE: This is used in the Queue implementation if we overwrite elements that have already been dequeued.
//       In this particular case it would make no sense for the new length of the vector to be greater than the old length, 
//       therefore I included the assertion below.
void
Vector_SetLength( size_t const NewLength, 
	          struct Vector *Vector )
{
	assert( NewLength < Vector->Length && "New length of vector must be less than that of the previous length" );

	Vector->Length = NewLength;
	return;
}


size_t
Vector_GetLength( struct Vector const *Vector )
{
	return Vector->Length;
}


size_t
Vector_GetCapacity( struct Vector const *Vector )
{
	return Vector->Capacity;
}

