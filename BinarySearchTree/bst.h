#ifndef BST_H_
#define BST_H_

struct BinarySearchTree;
struct BSTNode;

struct BinarySearchTree *BST_Initialize( void );

uint8_t BST_IsEmpty( struct BinarySearchTree const * );

void BST_Insert( void const *, 
		 struct BinarySearchTree *,
		 int8_t(*)(void const *, void const *) );


void BST_PreOrderTraversal( struct BinarySearchTree const *,
			    void(*)(void *)  );

void BST_InOrderTraversal( struct BinarySearchTree const *,
			   void(*)(void *)  );

void BST_PostOrderTraversal( struct BinarySearchTree const *,
			     void(*)(void *) );

void BST_LevelOrderTraversal( struct BinarySearchTree const *,
			      void(*)(void *)  );

void BST_Free( struct BinarySearchTree *, 
	       void(*FreeFn)(void *) );
				   


#endif
