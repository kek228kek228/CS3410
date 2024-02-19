#include <stdio.h>
#include <stdlib.h>
#include "treeTraversal.h"

int main()
{
  	char preorder[100], inorder[100];

    printf("Input the pre-order traversal: ");

    scanf ("%s", preorder);

    printf("Input the in-order traversal: ");

    scanf ("%s", inorder);

   int len;
   len=0;
   while(preorder[len]!='\0'){
      len++;
   }
   node_t *tree=build_tree2(inorder,preorder,0,len-1,0, len-1);

    //TODO: Calculate the length of input strings

    //TODO: Build the binary tree

    printf("The post-order traversal is: ");
    print_postorder(tree);
    //TODO: Output the post-order of the binary tree

    printf("\n");

    return 0;
}
