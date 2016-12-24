#include <stdio.h>#include <stdlib.h>#include <stdint.h>//#include "vector_queue.h"  // my implementation of a Queue built on top of a Vector/* The BST_LevelOrderSearch() function requires the use of a Queue to temporarily store the nodes of Binary Search Tree. * I have implemented a Queue in the Queue folder with the associated header "vector_queue.h". The Queue is built on-top of * a simple Vector implementation which has the header file "vector.h".  * * To build you would need the following: gcc -WAll -o src src.c vector_queueue.c vector.c -std=c99  *   Provided all the source files were in the same directory, or you could load each of the files from their respective locations * * With each source file I also run Valgrind with valgrind: --leak-check-yes ./src  */// BST -- Binary Search Tree  struct BSTNode {    void *Data;                    struct BSTNode *LeftSubTree;    struct BSTNode *RightSubTree; };  struct BinarySearchTree {    struct BSTNode *Root; };  struct BinarySearchTree * BST_Initialize( void ) {    struct BinarySearchTree *NewBinarySearchTree;        NewBinarySearchTree = (struct BinarySearchTree *)malloc( sizeof *NewBinarySearchTree );    if ( !NewBinarySearchTree )     {        printf( "Allocation error within BST_Initialize...terminating\n" );        exit( 1 );    }        NewBinarySearchTree->Root = NULL;        return NewBinarySearchTree; }  // Cases: //  1. Empty Tree -> Need to create the root node //  2. Populated Tree -> Find insert position and place BSTNode containing SourceData        static voidBST_InsertNewTreeNode( struct BSTNode *NewBSTNode,                                               struct BSTNode *CurrentTreeNode,                       int8_t (*CompareFn)(void const *, void const *) ){       if ( CompareFn( NewBSTNode->Data, CurrentTreeNode->Data ) <= 0 )     {        if ( CurrentTreeNode->LeftSubTree == NULL )         {            CurrentTreeNode->LeftSubTree = NewBSTNode;            return;        }        else        {            // Restart the process and do it again with the left subtree            BST_InsertNewTreeNode( NewBSTNode, CurrentTreeNode->LeftSubTree, CompareFn );        }    }    if ( CompareFn( NewBSTNode->Data, CurrentTreeNode->Data ) > 0 )    {        if ( CurrentTreeNode->RightSubTree == NULL )        {            CurrentTreeNode->RightSubTree = NewBSTNode;            return;        }        else        {            // Restart the process and do it again with the right subtree            BST_InsertNewTreeNode( NewBSTNode, CurrentTreeNode->RightSubTree, CompareFn );        }    }    }  static struct BSTNode * BST_CreateBSTNode( void const *SourceData ) {    struct BSTNode *NewBSTNode;        NewBSTNode = (struct BSTNode *)malloc( sizeof *NewBSTNode );    if ( !NewBSTNode )     {        printf( "Allocation error within BST_CreateBSTNode...terminating\n" );        exit( 1 );    }        NewBSTNode->Data = SourceData;  // removes the const qualifier    NewBSTNode->LeftSubTree = NewBSTNode->RightSubTree = NULL;        return NewBSTNode; }  uint8_t BST_IsEmpty( struct BinarySearchTree const *Tree ) {    return Tree->Root == NULL; }   void  BST_Insert( void *SourceData,              struct BinarySearchTree *Tree,             int8_t (*CompareFn)(void const *, void const *) ) {    struct BSTNode *NewBSTNode;        NewBSTNode = BST_CreateBSTNode( SourceData );    if ( BST_IsEmpty( Tree ) )    {        Tree->Root = NewBSTNode;        return;    }        BST_InsertNewTreeNode( NewBSTNode, Tree->Root, CompareFn );        return;}int8_t BST_Compare( void const *v1, void const *v2 ){    uint8_t *d1 = (uint8_t *)v1;    uint8_t *d2 = (uint8_t *)v2;        // if we did return d1 - d2 and d1 < d2 then the return value would be negative and cast to a positive integer     int16_t d1_scaled = (int16_t)*d1;  // int16_t can hold all values of an uint8_t plus we don't run into the same problem as before    int16_t d2_scaled = (int16_t)*d2;      return d1_scaled - d2_scaled; // this can return a negative, 0, or positive  }// Something is wrong with this search functionvoidBST_PreOrderTraversal( struct BSTNode const *CurrentTreeNode ){    if ( CurrentTreeNode != NULL )                              // initially wrote with while, needed to be an if    {        printf( "%u\n", *(uint8_t *)CurrentTreeNode->Data );        BST_PreOrderTraversal( CurrentTreeNode->LeftSubTree );        BST_PreOrderTraversal( CurrentTreeNode->RightSubTree );    }    return;}voidBST_InOrderTraversal( struct BSTNode const *CurrentTreeNode ){    if ( CurrentTreeNode != NULL )     {        BST_InOrderTraversal( CurrentTreeNode->LeftSubTree );        printf( "%u\n", *(uint8_t *)CurrentTreeNode->Data );        BST_InOrderTraversal( CurrentTreeNode->RightSubTree );    }        return;}voidBST_PostOrderTraversal( struct BSTNode const *CurrentTreeNode ){    if ( CurrentTreeNode != NULL )    {        BST_PostOrderTraversal( CurrentTreeNode->LeftSubTree );        BST_PostOrderTraversal( CurrentTreeNode->RightSubTree );        printf( "%u\n", *(uint8_t *)CurrentTreeNode->Data );     }    return;}/* Need to use my Queue Implemenation here */voidBST_LevelOrderTraversal( struct BSTNode const *CurrentTreeNode ){    struct VectorQueue *Queue;                // need a queue to temporarily store the nodes as we descend the tree        VectorQueue_Enqueue( &CurrentTreeNode );    while ( !VectorQueue_IsEmpty( Queue ) )    {        struct BSTNode *DequeuedTreeNode;                DequeuedTreeNode = *(struct BSTNode **)VectorQueue_Dequeue( Queue );  // PoppedTreeNode only borrows ownership of the Node, not responsible for free'ing        printf( "%u\n", *(uint8_t *)DequeuedTreeNode->Data );                if ( DequeuedTreeNode->LeftSubTree != NULL )         {            VectorQueue_Enqueue( DequeuedTreeNode->LeftSubTree );        }                if ( DequeuedTreeNode->RightSubTree != NULL )        {            VectorQueue_Enqueue( DequeuedTreeNode->RightSubTree );        }    }        VectorQueue_Free( Queue );        return;}  int32_t main( void ) {     struct BinarySearchTree *Tree;          Tree = BST_Initialize();     uint8_t *Value;          Value = (uint8_t *)malloc( sizeof *Value );     *Value = 12;     BST_Insert( Value, Tree, BST_Compare );          Value = (uint8_t *)malloc( sizeof *Value );     *Value = 9;     BST_Insert( Value, Tree, BST_Compare );          Value = (uint8_t *)malloc( sizeof *Value );     *Value = 7;     BST_Insert( Value, Tree, BST_Compare );          Value = (uint8_t *)malloc( sizeof *Value );     *Value = 14;     BST_Insert( Value, Tree, BST_Compare );          // The setup seems okay, something wrong with the Search Function     puts( "PreOrderTraversal" );     BST_PreOrderTraversal( Tree->Root );     putchar( '\n' );          puts( "InOrderTraversal" );     BST_InOrderTraversal( Tree->Root );     putchar( '\n' );          puts( "PostOrderTraversal" );     BST_PostOrderTraversal( Tree->Root );     putchar( '\n' );          puts( "LevelOrderTraversal" );     BST_LevelOrderTraversal( Tree->Root );     putchar( '\n' );          return 0;           }