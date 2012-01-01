/* 
 * File:   BinaryTree.h
 * Author: machiry
 *
 * Created on November 28, 2011, 12:27 PM
 */

#ifndef BINARYTREE_H
#define	BINARYTREE_H

#ifdef	__cplusplus
extern "C" {
#endif

    //This is the structure used for each node of the tree
    typedef struct TreeNode{
        int data;
        struct TreeNode *left;
        struct TreeNode *right;
    } Node,*TreeNodePtr;

    //This is the structure used to store the tree
    typedef struct Tree{
        struct TreeNode *root;
        ulong size;
    } TreeRootNode,*TreePtr;

    //This function creates a new node in the tree
    //returns: created TreeNodePtr or null if this function is unable to allocate the required space
    TreeNodePtr GetNewTreeNode();

    //This function creates a new Tree
    //returns: created TreePtr or null if this function is unable to allocate the required space
    TreePtr GetNewTree();

    //This function inserts the provided data i.e newData in to the given tree
    //returns:enum ErrorCode(refer dserror.h) depending on the result
    int InsertNode(TreePtr tree,int newData);

    //This function deletes the node(s) which have the provided data from teh given btree
    //This function also restores the ordering of the nodes. in short this function balances the tree
    //after deleting the node
    //if removeAllNodes is non-zero then all the nodes having the data will be deleted else
    //only first node encountered will be deleted
    //returns:enum ErrorCode(refer dserror.h) depending on the result
    int RemoveNodeByData(TreePtr tree,int data,int removeAllNodes);

    //This function deletes the node pointed by node.
    //As the function above this function also restores the ordering of the nodes. in short this function balances the tree
    //returns:enum ErrorCode(refer dserror.h) depending on the result
    int RemoveNodeByNodePtr(TreePtr tree,TreeNodePtr node);

    //This function traverses the provided tree in inorder i.e Left,Root,Right
    //returns: Integer array containing the elements of the tree in InOrder or null either if there is not available space or
    //provided tree is empty
    int* GetTreeByInOrder(TreePtr tree);

    //Same as above function but returns the tree in PreOrder i.e Root,Left,Right
    int* GetTreeByPreOrder(TreePtr tree);

    //Same as above function but returns the tree in PostOrder i.e Left,Right,Root
    int* GetTreeByPostOrder(TreePtr tree);

    //Finds the first node in tree with the provided data
    //returs: found pointer to the node or null either if there is no node with the provided data
    //or tree is null
    TreeNodePtr FindFirstNode(TreePtr tree,int data);


    //Note:All the below functions retuns null if provided tree is null


    //This Function finds the first Predecessor of the data
    //in PreOrder traversal of the tree
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetPreOrderPredecessorByValue(TreePtr tree,int data);

    //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetPreOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr);


    //This Function finds the first Sucessor of the data
    //in PreOrder traversal of the tree
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetPreOrderSucessorByValue(TreePtr tree,int data);

     //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetPreOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr);


    //This Function finds the first Predecessor of the data
    //in PostOrder traversal of the tree
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetPostOrderPredecessorByValue(TreePtr tree,int data);

    //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetPostOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr);


    //This Function finds the first Sucessor of the data
    //in PostOrder traversal of the tree
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetPostOrderSucessorByValue(TreePtr tree,int data);

    //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetPostOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr);


    //This Function finds the first Predecessor of the data
    //in InOrder traversal of the tree
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetInOrderPredecessorByValue(TreePtr tree,int data);

    //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the first node in the traversal
    TreeNodePtr GetInOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr);

    //This Function finds the first Sucessor of the data
    //in InOrder traversal of the tree
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetInOrderSucessorByValue(TreePtr tree,int data);
  

    //Same as the above function but this searches provided node ptr dataPtr
    //returns: the pointer to the node or null if this is the last node in the traversal
    TreeNodePtr GetInOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr);

    //This function clones the tree pointed by the tree
    //returns: newly created tree with same data as the provided tree.
    TreePtr CloneTree(TreePtr tree);

    //The below functions are tree transformation functions which transform the provided tree
    //in to different structure

    //This function converts the tree whose root is pointed by root
    //to doubly linked list:
    //--->elements in the list will be in InOrder traversal order of the tree
    //--->left and right of each node in the resulting structure will be analgous to
    //--->next and prev(or back) of the usual doubly linked list structure
    //returns: tree having the structure as mentioned above
    TreeNodePtr ConvertTreeToDLList(TreeNodePtr root);

    //This functions returns the tree in doubly linked list structure
    //Here the ordering of elements will be: according to their level in the tree
    //Eg: if the tree is
    //<---------------9<-----
    //--------->5----------->15--->
    //------3<-----7<------------19<--
    //--->1-->4-->6----------->17--->20-->
    //The returned transformed list contains elements in the order pointed by the arrow (--> , <--)
    //9->5->15->19->7->3->1->4->6->17->20
    TreeNodePtr FlattenTree(TreePtr tree);

#ifdef	__cplusplus
}
#endif

#endif	/* BINARYTREE_H */

