/* Approach: This source file contains an implementation of a generic Vector (or growable array).  
 *           The Vector can be used as a standalone implementation, however, it's intended purpose for me was to serve as a backend buffer for a Queue implementation.
 *          
 * Benefits: This approach is very flexible in the fact that you can have more than one Vector of either the same type or different type in the same program. Also, it is fairly simple to use with 
 *           a small API and simple resource ownership semantics.
 *
 * Drawbacks: The generic nature of the Vector makes the implementation slower than one that handles a specific type only. All data on the Vector is heap allocated and the indirection has an obvious
 *            performance cost. Also, to make resource management as straightforward as possible, "popping" data off the Vector creates a new copy of the element instead of returning a pointer to the 
 *            vector element. This is definitely slows down performance since we copy the data, then now have to free the data as well. However, the big benefit is that ownership and resource management
 *            are easier since when you pop an element onto the Vector the Vector now owns the resource and will free it and when you pop an element you become responsible for cleaning up the resource.
 *            That is all there is to it. Returning pointers to the elements would be faster but would greatly complicate when a resource should be freed especially if the user is constantly pushing
 *            and popping data on and off the vector respectively.
 *
 * Responsibilities of the User:
 *  1. Define a function in the program to free the elements located on the vector: void(*FreeFn)(void *) -- example in any of the vector_sample#.c files
 *      If you have more than one Vector, each with unique elements, you should define a freeing function for each and make the appropriate call 
 *  2. Save the return value from a call to Vector_Pop() and make sure to free the resource to prevent a memory leak
 *  3. Set the struct Vector *Object to NULL following the all to Vector_Free() to prevent erroneous operations like Vector_Push() or Vector_Pop() after freeing the Vector
 */


#ifndef VECTOR_H_
#define VECTOR_H_

struct Vector;

struct Vector *Vector_Initialize( size_t const );

struct Vector *Vector_InitializeWithCapacity( size_t const, size_t const  );

void Vector_Push( void const *, struct Vector * );

uint8_t Vector_IsFull( struct Vector const * );

void *Vector_Pop( struct Vector * );

uint8_t Vector_IsEmpty( struct Vector const * );

void *Vector_AtIndex( size_t const, struct Vector const * );

void Vector_Free( struct Vector *, void(*)(void *) );

void Vector_SetLength( size_t const, struct Vector * );

size_t Vector_GetLength( struct Vector const * );

size_t Vector_GetCapacity( struct Vector const * );

#endif 

