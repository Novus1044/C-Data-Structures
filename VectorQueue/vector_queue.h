#include "vector.h"

#ifndef VECTOR_QUEUE_H_
#define VECTOR_QUEUE_H_

struct VectorQueue;

/* SECTION: Initializers */
struct VectorQueue *VectorQueue_Initialize( void );

struct VectorQueue *VectorQueue_InitializeWithCapacity( size_t const );

/* SECTION: Clean-up */
void VectorQueue_Free( struct VectorQueue *, void(*)(void *) );


/* SECTION: Main Operations */
void VectorQueue_Enqueue( void const *, struct VectorQueue * );

void *VectorQueue_Dequeue( struct VectorQueue * );


/* SECTION: Predicates */
uint8_t VectorQueue_IsFull( struct VectorQueue const * );

uint8_t VectorQueue_IsEmpty( struct VectorQueue const * );



/* SECTION: Additional Functionaliy */
size_t VectorQueue_GetLength( struct VectorQueue const * );

size_t VectorQueue_GetCapacity( struct VectorQueue const * );

#endif
