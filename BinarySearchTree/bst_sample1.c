#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bst.h"
#include "vector_queue.h"

int8_t
CompareFn( void const *v1, void const *v2 )
{
        uint8_t d1 = *(uint8_t *)v1;
        uint8_t d2 = *(uint8_t *)v2;

        return (int16_t)d1 - (int16_t)d2;  // if d1 < d2, d1 - d2 < 0 but since these are uint8_t the value will wrap, therefore casted to a larger type first
}

void 
Cleanup( void *TreeNodeData )
{
	free( TreeNodeData );
	return;
}

void
Print( void *DataElement )
{
	printf( "%u\n", *(uint8_t *)DataElement );
	return;
}

void
PopulateTree( struct BinarySearchTree *Tree )
{
	uint8_t TreeDataValues[] = {12, 9, 7, 14};
	size_t const TreeDataValuesLength = (sizeof TreeDataValues) / (sizeof TreeDataValues[0]);

	for (size_t i = 0; i < TreeDataValuesLength; ++i)
	{
		uint8_t *Value = (uint8_t *)malloc( sizeof *Value );

		if ( Value ) 
		{
			*Value = TreeDataValues[i];
			BST_Insert( Value, Tree, CompareFn );
		}
		else
		{
			puts( "Well crap!" );
		}
	}

	return;
}

int32_t
main( void )
{
	struct BinarySearchTree *Tree;
	
	Tree = BST_Initialize();
	
	PopulateTree( Tree );

	puts( "PreOrderTraversal" );
	BST_PreOrderTraversal( Tree, Print );
	putchar( '\n' );

	puts( "InOrderTraversal" );
	BST_InOrderTraversal( Tree, Print );
	putchar( '\n' );

	puts( "PostOrderTraversal" );
	BST_PostOrderTraversal( Tree, Print );
     	putchar( '\n' );

     	puts( "LevelOrderTraversal" );           // Queue implementation is used here
     	BST_LevelOrderTraversal( Tree, Print );
     	putchar( '\n' );

	BST_Free( Tree, Cleanup );
	Tree = NULL;

     	return 0;
 }

