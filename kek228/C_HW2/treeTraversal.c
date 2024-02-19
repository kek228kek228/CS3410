#include <stdio.h>
#include <stdlib.h>
#include "treeTraversal.h"

 /** Helper function that allocates a new node with the
  * given data and NULL left and right pointers. 
  * We have implemented this function for your conveninece becuase this function uses malloc that you probably are not familiar with.
  * Don't forget to free all memory allocted here before your program terminates. */
node_t *newNode(char data)
{
  node_t *node = (node_t *)malloc(sizeof(node_t));
  node->data = data;
  node->left = NULL;
  node->right = NULL;

  return(node);
}

/**
 * TODO: Implement functions defined in treeTraversal.h here.
 */
 
