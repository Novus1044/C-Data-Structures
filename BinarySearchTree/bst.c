#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#include "vector_queue.h"  // my implementation of a Queue built on top of a Vector
 

/* The BST_LevelOrderSearch() function requires the use of a Queue to temporarily store the nodes of Binary Search Tree.
 * I have implemented a Queue in the Queue folder with the associated header "vector_queue.h". The Queue is built on-top of
 * a simple Vector implementation which has the header file "vector.h".
 *
 * To build you would need the following: gcc -WAll -o src src.c vector_queueue.c vector.c -std=c99
 *   Provided all the source files were in the same directory, or you could load each of the files from their respective locations
 *
 * With each source file I also run Valgrind with valgrind: --leak-check-yes ./src
 */


// BST -- Binary Search Tree
 struct BSTNode {
    void *Data;               
    struct BSTNode *LeftSubTree;
    struct BSTNode *RightSubTree;
};


struct BinarySearchTree {
    struct BSTNode *Root;
};


struct BinarySearchTree *
BST_Initialize( void )
{
    struct BinarySearchTree *NewBinarySearchTree = (struct BinarySearchTree *)malloc( sizeof *NewBinarySearchTree );
   
    if ( NewBinarySearchTree )
    {
	    NewBinarySearchTree->Root = NULL;
	    return NewBinarySearchTree;
    }
    else
    {
	  puts( "Unable to initialize the tree..terminating" );
	  exit( 1 );
    }
} 


// Cases:
//  1. Empty Tree -> Need to create the root node
//  2. Populated Tree -> Find insert position and place BSTNode containing SourceData
static void
BST_InsertNewTreeNode( struct BSTNode const *restrict NewBSTNode,                       
                       struct BSTNode *restrict CurrentTreeNode,
                       int8_t (*CompareFn)(void const *, void const *) )
{  
    if ( CompareFn( NewBSTNode->Data, CurrentTreeNode->Data ) <= 0 )
    {
        if ( CurrentTreeNode->LeftSubTree == NULL )
        {
            CurrentTreeNode->LeftSubTree = NewBSTNode;
            return;
        }
        else
        {
            // Restart the process and do it again with the left subtree
            BST_InsertNewTreeNode( NewBSTNode, CurrentTreeNode->LeftSubTree, CompareFn );
        }
    }
    if ( CompareFn( NewBSTNode->Data, CurrentTreeNode->Data ) > 0 )
    {
        if ( CurrentTreeNode->RightSubTree == NULL )
        {
            CurrentTreeNode->RightSubTree = NewBSTNode;
            return;
        }
        else
        {
            // Restart the process and do it again with the right subtree
            BST_InsertNewTreeNode( NewBSTNode, CurrentTreeNode->RightSubTree, CompareFn );

        }
    }
}


static struct BSTNode *
BST_CreateBSTNode( void const *SourceData )
{
    struct BSTNode *NewBSTNode = (struct BSTNode *)malloc( sizeof *NewBSTNode );
    
    if ( NewBSTNode )
    {
	  NewBSTNode->Data = SourceData; // removes the const qualifier
	  NewBSTNode->LeftSubTree = NewBSTNode->RightSubTree = NULL; 
	  	
	  return NewBSTNode;
    }
    else 
    {
	  printf( "Allocation error within BST_CreateBSTNode...terminating\n" );
	  exit( 1 );
    }

}

inline __attribute__((always_inline))
uint8_t
BST_IsEmpty( struct BinarySearchTree const *Tree )
{
	return Tree->Root == NULL;
}


void
BST_Insert( void const *SourceData,
            struct BinarySearchTree *Tree,
            int8_t (*CompareFn)(void const *, void const *) )
{
	struct BSTNode *NewBSTNode =  BST_CreateBSTNode( SourceData );
    
	if ( BST_IsEmpty( Tree ) )
	{
		Tree->Root = NewBSTNode;
	}
    	else 
   	{ 
		BST_InsertNewTreeNode( NewBSTNode, Tree->Root, CompareFn );
    	}

    return;
}

/************* THIS NEEDS FIXED TO TAKE THE TREE ********** -- each needs a sub function to recursively iterate */
static void
BST_RecursivePreOrderTraversal( struct BSTNode const *CurrentTreeNode,
				void(*DerefFn)(void *) )
{
	if ( CurrentTreeNode != NULL )                             
	{	    
		DerefFn( CurrentTreeNode->Data );	
		BST_RecursivePreOrderTraversal( CurrentTreeNode->LeftSubTree, DerefFn );
		BST_RecursivePreOrderTraversal( CurrentTreeNode->RightSubTree, DerefFn );
    	}
    
    	return;
}

void
BST_PreOrderTraversal( struct BinarySearchTree const *Tree,
		       void(*DerefFn)(void *) )
{
	BST_RecursivePreOrderTraversal( Tree->Root, DerefFn );
	return;	
}

 
static void
BST_RecursiveInOrderTraversal( struct BSTNode const *CurrentTreeNode,	
			       void(*DerefFn)(void *)  )
{
    if ( CurrentTreeNode != NULL )
    {
	    BST_RecursiveInOrderTraversal( CurrentTreeNode->LeftSubTree, DerefFn );
	    DerefFn( CurrentTreeNode->Data ); 
	    BST_RecursiveInOrderTraversal( CurrentTreeNode->RightSubTree, DerefFn );
    }
    
    return;
}

void 
BST_InOrderTraversal( struct BinarySearchTree const *Tree,
		      void(*DerefFn)(void *) )
{
	BST_RecursiveInOrderTraversal( Tree->Root, DerefFn );
	return;
}

 
static void
BST_RecursivePostOrderTraversal( struct BSTNode const *CurrentTreeNode,
				 void(*DerefFn)(void *)  )
{
    if ( CurrentTreeNode != NULL )
    {
	    BST_RecursivePostOrderTraversal( CurrentTreeNode->LeftSubTree, DerefFn );
	    BST_RecursivePostOrderTraversal( CurrentTreeNode->RightSubTree, DerefFn );
	    DerefFn( CurrentTreeNode->Data );
    }

    return;
}

void
BST_PostOrderTraversal( struct BinarySearchTree const *Tree,
			void(*DerefFn)(void *) )
{
	BST_RecursivePostOrderTraversal( Tree->Root, DerefFn );
	return;
}


void
BST_LevelOrderTraversal( struct BinarySearchTree const *Tree, 
			 void(*DerefFn)(void *) )
	
{
	struct BSTNode const *CurrentTreeNode = Tree->Root;
	
	struct VectorQueue *Queue = VectorQueue_Initialize();             // need a queue to temporarily store the nodes as we descend the tree

	VectorQueue_Enqueue( CurrentTreeNode, Queue );

	while ( !VectorQueue_IsEmpty( Queue ) )
	{
	       	struct BSTNode *DequeuedTreeNode = VectorQueue_Dequeue( Queue );  // PoppedTreeNode only borrows ownership of the Node, not responsible for free'ing

		DerefFn( DequeuedTreeNode->Data );	

		if ( DequeuedTreeNode->LeftSubTree != NULL )
		{
			VectorQueue_Enqueue( DequeuedTreeNode->LeftSubTree, Queue );
		}

		if ( DequeuedTreeNode->RightSubTree != NULL )
		{
			VectorQueue_Enqueue( DequeuedTreeNode->RightSubTree, Queue );
		}
	}
	
	VectorQueue_Free( Queue, NULL );
	Queue = NULL;

	return;
}


// Something is wrong with this!!!
static void
BST_FreeAllBSTNodes( struct BSTNode *CurrentTreeNode,
                     void(*FreeFn)(void *) )
{
        if ( CurrentTreeNode != NULL )
        {
                BST_FreeAllBSTNodes( CurrentTreeNode->LeftSubTree, FreeFn );
                BST_FreeAllBSTNodes( CurrentTreeNode->RightSubTree, FreeFn );

                FreeFn( CurrentTreeNode->Data );
                free( CurrentTreeNode );
        }

        return;
}


void 
BST_Free( struct BinarySearchTree *Tree,
       	  void(*FreeFn)(void *)	)
{
	if ( FreeFn )
	{
		BST_FreeAllBSTNodes( Tree->Root, FreeFn );
	}
	free( Tree );

	return;
	
}




 
