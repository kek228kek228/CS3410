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
node_t *build_tree(char in[], char pre[], int start, int end){
  return newNode(in[start]);  
}

node_t *build_tree2(char in[], char pre[], int in_left, int in_right, int pre_left, int pre_right){
  int root;
  node_t *node=newNode(pre[pre_left]);
  for (int i=in_left; i<=in_right; i++){
    if(pre[pre_left]==in[i]){
      root=i;
    }
  }
  if(root!=in_left){
    node->left=build_tree2(in,pre,in_left, root-1, pre_left+1, pre_left+root-in_left);
  }
  if(root!=in_right){
    node->right=build_tree2(in,pre,root+1, in_right, pre_right-(in_right-root-1), pre_right);

  }
  return (node);  
}

void print_postorder(node_t *node){
  if (node == NULL){
    return; 
  }  
     // first recur on left subtree 
  print_postorder(node->left); 
  
     // then recur on right subtree 
  print_postorder(node->right); 
  
     // now deal with the node 
  printf("%c", node->data);
  free(node);
}
/**
 * TODO: Implement functions defined in treeTraversal.h here.
 */
 
