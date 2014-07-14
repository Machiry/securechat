#include "CommonHeader.h"
#include "btree.h"
#include "dserror.h"

TreeNodePtr GetNewTreeNode() {
    TreeNodePtr treeNode = (TreeNodePtr) malloc(sizeof (Node));
    if (treeNode) {
        treeNode->data = 0;
        treeNode->left = treeNode->right = null;
    }

    return treeNode;
}

TreePtr GetNewTree() {
    TreePtr treeRoot = (TreePtr) malloc(sizeof (TreeRootNode));
    if (treeRoot) {
        treeRoot->size = 0;
        treeRoot->root = null;
    }
    return treeRoot;
}

int InsertNode(TreePtr tree, int newData) {
    ErrorCode err = INSUFFICIENTSPACE;
    if (tree != null) {
        TreeNodePtr newNode = GetNewTreeNode();
        if (newNode) {
            newNode->data = newData;
            newNode->left = newNode->right = null;
            //Here this is to make sure that we don't loop infinately for cyclic trees
            unsigned long maxNoOFNodes = tree->size;
            TreeNodePtr currentNode = tree->root;
            if (currentNode == null) {
                tree->root = newNode;
                tree->size = tree->size + 1;
                return SUCESS;
            }
            while (maxNoOFNodes > 0 && currentNode != null) {
                if (currentNode->data < newNode->data) {
                    if (currentNode->right == null) {
                        currentNode->right = newNode;
                        tree->size = tree->size + 1;
                        err = SUCESS;
                        break;
                    } else {
                        currentNode = currentNode->right;
                    }
                } else {
                    if (currentNode->left == null) {
                        currentNode->left = newNode;
                        tree->size = tree->size + 1;
                        err = SUCESS;
                        break;
                    } else {
                        currentNode = currentNode->left;
                    }
                }
                maxNoOFNodes--;
            }
        }

    }

    return err;
}

int RemoveNodeByData(TreePtr tree, int data, int removeAllNodes) {
    ErrorCode err = UNSUCCESSFUL;
    if (tree) {
        TreeNodePtr currentNodePtr = tree->root;
        TreeNodePtr parentNode = null;
        unsigned long MaxNoOfIterations = tree->size;
        while (currentNodePtr != null && MaxNoOfIterations > 0) {
            if (currentNodePtr->data == data) {
                TreeNodePtr resultNodePtr = currentNodePtr->right ? currentNodePtr->right : currentNodePtr->left;
                TreeNodePtr nextSearchNode = currentNodePtr->left;
                TreeNodePtr nextSeachNodeParentPtr = currentNodePtr->right ? currentNodePtr->right : parentNode;
                if (currentNodePtr->left && currentNodePtr->right) {
                    TreeNodePtr tempTreeNodePtr = currentNodePtr->right;
                    while (tempTreeNodePtr->left) {
                        tempTreeNodePtr = tempTreeNodePtr->left;
                    }
                    tempTreeNodePtr->left = nextSearchNode;
                    nextSeachNodeParentPtr = tempTreeNodePtr;
                }

                if (parentNode != null) {
                    if (parentNode->left == currentNodePtr) {
                        parentNode->left = resultNodePtr;
                    } else {
                        parentNode->right = resultNodePtr;
                    }
                } else {
                    tree->root = resultNodePtr;
                }
                free(currentNodePtr);
                err = SUCESS;
                tree->size = tree->size - 1;
                if (!removeAllNodes) {
                    break;
                }
                parentNode = nextSeachNodeParentPtr;
                currentNodePtr = nextSearchNode;
            } else {
                parentNode = currentNodePtr;
                if (currentNodePtr->data < data) {
                    currentNodePtr = currentNodePtr->right;
                } else {
                    currentNodePtr = currentNodePtr->left;
                }
            }
            MaxNoOfIterations--;
        }
    }
    return err;
}

int RemoveNodeByNodePtr(TreePtr tree, TreeNodePtr node) {
    ErrorCode err = UNSUCCESSFUL;
    if (tree && node) {
        TreeNodePtr currentNodePtr = tree->root;
        TreeNodePtr parentNode = null;
        unsigned long MaxNoOfIterations = tree->size;
        while (currentNodePtr != null && MaxNoOfIterations > 0) {
            if (currentNodePtr == node) {
                TreeNodePtr resultNodePtr = currentNodePtr->right ? currentNodePtr->right : currentNodePtr->left;
                if (currentNodePtr->left && currentNodePtr->right) {
                    TreeNodePtr tempTreeNodePtr = currentNodePtr->right;
                    while (tempTreeNodePtr->left) {
                        tempTreeNodePtr = tempTreeNodePtr->left;
                    }
                    tempTreeNodePtr->left = currentNodePtr->left;
                }

                if (parentNode != null) {
                    if (parentNode->left == currentNodePtr) {
                        parentNode->left = resultNodePtr;
                    } else {
                        parentNode->right = resultNodePtr;
                    }
                } else {
                    tree->root = resultNodePtr;
                }
                free(currentNodePtr);
                err = SUCESS;
                tree->size = tree->size - 1;
                break;
            } else {
                parentNode = currentNodePtr;
                if (currentNodePtr->data < node->data) {
                    currentNodePtr = currentNodePtr->right;
                } else {
                    currentNodePtr = currentNodePtr->left;
                }
            }
            MaxNoOfIterations--;
        }
    }
    return err;
}

static int GetTreeByInOrderInPlace(TreeNodePtr root, int *arr, long maxIndex, int *myIndex) {
    ErrorCode err = SUCESS;
    if (!root) {
        return err;
    }
    if ((*myIndex) >= maxIndex) {
        return UNSUCCESSFUL;
    }
    err = GetTreeByInOrderInPlace(root->left, arr, maxIndex, myIndex);
    if (err || ((*myIndex) >= maxIndex)) {
        return UNSUCCESSFUL;
    }

    arr[*myIndex] = root->data;
    (*myIndex) = (*myIndex) + 1;
    err = GetTreeByInOrderInPlace(root->right, arr, maxIndex, myIndex);

    return err || SUCESS;
}

int* GetTreeByInOrder(TreePtr tree) {
    if (!tree) {
        return null;
    }
    int* resultArray = (int*) malloc(tree->size * sizeof (int));
    int lastIndex = 0;
    if (GetTreeByInOrderInPlace(tree->root, resultArray, tree->size, &lastIndex) || lastIndex != tree->size) {
        free(resultArray);
        return null;
    }
    return resultArray;
}

static int GetTreeByPreOrderInPlace(TreeNodePtr root, int *arr, long maxIndex, int *myIndex) {
    ErrorCode err = SUCESS;
    if (!root) {
        return err;
    }
    if ((*myIndex) >= maxIndex) {
        return UNSUCCESSFUL;
    }
    arr[*myIndex] = root->data;
    (*myIndex) = (*myIndex) + 1;

    err = GetTreeByPreOrderInPlace(root->left, arr, maxIndex, myIndex);
    err = err || GetTreeByPreOrderInPlace(root->right, arr, maxIndex, myIndex);

    return err;
}

static int GetTreeByPostOrderInPlace(TreeNodePtr root, int *arr, long maxIndex, int *myIndex) {
    ErrorCode err = SUCESS;
    if (!root) {
        return err;
    }

    err = GetTreeByPostOrderInPlace(root->left, arr, maxIndex, myIndex);
    err = err || GetTreeByPostOrderInPlace(root->right, arr, maxIndex, myIndex);

    if (err || ((*myIndex) >= maxIndex)) {
        return UNSUCCESSFUL;
    }

    arr[*myIndex] = root->data;
    (*myIndex) = (*myIndex) + 1;

    return err;
}

int* GetTreeByPreOrder(TreePtr tree) {
    if (!tree) {
        return null;
    }
    int* resultArray = (int*) malloc(tree->size * sizeof (int));
    int lastIndex = 0;
    if (GetTreeByPreOrderInPlace(tree->root, resultArray, tree->size, &lastIndex) || lastIndex != tree->size) {
        free(resultArray);
        return null;
    }
    return resultArray;
}

int* GetTreeByPostOrder(TreePtr tree) {
    if (!tree) {
        return null;
    }
    int* resultArray = (int*) malloc(tree->size * sizeof (int));
    int lastIndex = 0;
    if (GetTreeByPostOrderInPlace(tree->root, resultArray, tree->size, &lastIndex) || lastIndex != tree->size) {
        free(resultArray);
        return null;
    }
    return resultArray;
}

TreeNodePtr FindFirstNode(TreePtr tree, int data) {
    TreeNodePtr targetNodePtr = null;
    if (tree) {
        long maxNoOfIterations = tree->size;
        targetNodePtr = tree->root;
        while (targetNodePtr != null && maxNoOfIterations > 0) {
            if (targetNodePtr->data == data) {
                return targetNodePtr;
            }
            if (data > targetNodePtr->data) {
                targetNodePtr = targetNodePtr->right;
            } else {
                targetNodePtr = targetNodePtr->left;
            }
            maxNoOfIterations--;
        }
        return null;
    }
    return null;
}

TreeNodePtr GetInOrderSucessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            
            if(currentNode->data < data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                parentNode = currentNode;
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            targetNode = parentNode;
            if(sourceNode->right)
            {
                sourceNode = sourceNode->right;
                while(sourceNode->left)
                {
                    sourceNode = sourceNode->left;
                }
                targetNode = sourceNode;
            }
        }
     }
    return targetNode;
}

TreeNodePtr GetInOrderPredecessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < data)
            {
                parentNode = currentNode;
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            targetNode = parentNode;
            if(sourceNode->left)
            {
                sourceNode = sourceNode->left;
                while(sourceNode->right)
                {
                    sourceNode = sourceNode->right;
                }
                targetNode = sourceNode;
            }
        }
     }
    return targetNode;
}

TreeNodePtr GetPreOrderSucessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                if(currentNode->right)
                {
                    parentNode = currentNode;
                }
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(sourceNode->left)
            {
                targetNode = sourceNode->left;
            }
            else if(sourceNode->right)
            {
                targetNode = sourceNode->right;
            }
            else if(parentNode)
            {
                targetNode = parentNode->right;
            }
        }
     }
    return targetNode;
}

TreeNodePtr GetPreOrderPredecessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            parentNode = currentNode;
            if(currentNode->data < data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(parentNode)
            {
                targetNode = parentNode;
                if(parentNode->left == sourceNode)
                {
                    targetNode = parentNode;
                }
                else if(parentNode->left)
                {
                    TreeNodePtr tempNode = parentNode->left;
                    while(tempNode->right || tempNode->left)
                    {
                        tempNode = tempNode->right?tempNode->right:tempNode->left;
                    }
                    targetNode = tempNode;
                }
            }
        }
     }
    return targetNode;
}

TreeNodePtr GetPostOrderPredecessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < data)
            {
                if(currentNode->left)
                {
                    parentNode = currentNode;
                }
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(sourceNode->left || sourceNode->right)
            {
                targetNode = sourceNode->right?sourceNode->right:sourceNode->left;
                /*while(targetNode->left || targetNode->right)
                {
                    targetNode = targetNode->right?targetNode->right:targetNode->left;
                }*/
            }
            else if(parentNode)
            {
                targetNode = parentNode->left;
            }
        }
     }
    return targetNode;
}

TreeNodePtr GetPostOrderSucessorByValue(TreePtr tree,int data)
{
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode->data == data)
            {
                sourceNode = currentNode;
                break;
            }
            parentNode = currentNode;
            if(currentNode->data < data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(parentNode)
            {
                targetNode = parentNode;
                if(parentNode->left == sourceNode)
                {
                    if(!parentNode->right)
                    {
                        targetNode = parentNode;
                    }
                    else
                    {
                        TreeNodePtr tempNode = parentNode->right;
                        while(tempNode->right || tempNode->left)
                        {
                            tempNode = tempNode->left?tempNode->left:tempNode->right;
                        }
                        targetNode = tempNode;
                    }
                   
                }
            }
        }
     }
    return targetNode;
}


 TreeNodePtr GetPreOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }
            parentNode = currentNode;
            if(currentNode->data < dataPtr->data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(parentNode)
            {
                targetNode = parentNode;
                if(parentNode->left == sourceNode)
                {
                    targetNode = parentNode;
                }
                else if(parentNode->left)
                {
                    TreeNodePtr tempNode = parentNode->left;
                    while(tempNode->right || tempNode->left)
                    {
                        tempNode = tempNode->right?tempNode->right:tempNode->left;
                    }
                    targetNode = tempNode;
                }
            }
        }
     }
    return targetNode;
 }

 TreeNodePtr GetPreOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < dataPtr->data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                if(currentNode->right)
                {
                    parentNode = currentNode;
                }
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(sourceNode->left)
            {
                targetNode = sourceNode->left;
            }
            else if(sourceNode->right)
            {
                targetNode = sourceNode->right;
            }
            else if(parentNode)
            {
                targetNode = parentNode->right;
            }
        }
     }
    return targetNode;
 }

 TreeNodePtr GetPostOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < dataPtr->data)
            {
                if(currentNode->left)
                {
                    parentNode = currentNode;
                }
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(sourceNode->left || sourceNode->right)
            {
                targetNode = sourceNode->right?sourceNode->right:sourceNode->left;
                /*while(targetNode->left || targetNode->right)
                {
                    targetNode = targetNode->right?targetNode->right:targetNode->left;
                }*/
            }
            else if(parentNode)
            {
                targetNode = parentNode->left;
            }
        }
     }
    return targetNode;
 }

 TreeNodePtr GetPostOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }
            parentNode = currentNode;
            if(currentNode->data < dataPtr->data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            if(parentNode)
            {
                targetNode = parentNode;
                if(parentNode->left == sourceNode)
                {
                    if(!parentNode->right)
                    {
                        targetNode = parentNode;
                    }
                    else
                    {
                        TreeNodePtr tempNode = parentNode->right;
                        while(tempNode->right || tempNode->left)
                        {
                            tempNode = tempNode->left?tempNode->left:tempNode->right;
                        }
                        targetNode = tempNode;
                    }

                }
            }
        }
     }
    return targetNode;
 }
 
 TreeNodePtr GetInOrderPredecessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }
            if(currentNode->data < dataPtr->data)
            {
                parentNode = currentNode;
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            targetNode = parentNode;
            if(sourceNode->left)
            {
                sourceNode = sourceNode->left;
                while(sourceNode->right)
                {
                    sourceNode = sourceNode->right;
                }
                targetNode = sourceNode;
            }
        }
     }
    return targetNode;
 }
 
 TreeNodePtr GetInOrderSucessorByNode(TreePtr tree,TreeNodePtr dataPtr)
 {
    TreeNodePtr targetNode = null;
    if(tree != null && tree->root != null && tree->size > 0)
    {
        TreeNodePtr currentNode = tree->root;
        TreeNodePtr parentNode = null;
        TreeNodePtr sourceNode = null;
        long noOfIterations = 0;
        while(currentNode!=null && noOfIterations < tree->size)
        {
            if(currentNode == dataPtr)
            {
                sourceNode = currentNode;
                break;
            }

            if(currentNode->data < dataPtr->data)
            {
                currentNode = currentNode->right;
            }
            else
            {
                parentNode = currentNode;
                currentNode = currentNode->left;
            }
            noOfIterations++;

         }
        if(sourceNode)
        {
            targetNode = parentNode;
            if(sourceNode->right)
            {
                sourceNode = sourceNode->right;
                while(sourceNode->left)
                {
                    sourceNode = sourceNode->left;
                }
                targetNode = sourceNode;
            }
        }
     }
    return targetNode;
 }


TreeNodePtr ConvertTreeToDLList(TreeNodePtr root)
{
    TreeNodePtr dlfirstNode = null;
    //version 2 : More elegant
    if(root)
    {
        TreeNodePtr leftSubTree = null,rightSubTree= null,tailNode = null;
        leftSubTree = ConvertTreeToDLList(root->left);
        rightSubTree = ConvertTreeToDLList(root->right);
        root->left = rightSubTree;
        if(rightSubTree)
        {
            rightSubTree->right = root;
        }

        if(leftSubTree)
        {
            dlfirstNode = leftSubTree;
            tailNode = leftSubTree;
            while(tailNode->left)
            {
                tailNode = tailNode->left;
            }
            tailNode->left = root;
            root->right = tailNode;
        }
        else
        {
            dlfirstNode = root;
            root->right = null;
        }


    }
    return dlfirstNode;
}

TreeNodePtr CopyTree(TreeNodePtr root)
{
    if(root)
    {
        TreeNodePtr newRoot = GetNewTreeNode();
        newRoot->data = root->data;
        newRoot->left = CopyTree(root->left);
        newRoot->right = CopyTree(root->right);
        return newRoot;
    }
    return null;
}

TreePtr CloneTree(TreePtr tree)
{
    if(tree)
    {
        TreePtr newTree = GetNewTree();
        newTree->size = tree->size;
        newTree->root = CopyTree(tree->root);
        return newTree;
    }
    return null;
}

TreeNodePtr PressTree(TreeNodePtr *arr,int length,int straight)
{
    TreeNodePtr first = null,last=null;
    if(arr)
    {
        TreeNodePtr *tempArray = (TreeNodePtr*)malloc(length*2*sizeof(TreeNodePtr));
        TreeNodePtr tempNodePtr = null;
        int tempArrayLength = 0,index=0;

        if(straight)
        {
            tempArrayLength = 0;
            for(index = 0;index<length;index++)
            {
                if(arr[index])
                {
                    if(arr[index]->left)
                    {
                        tempArray[tempArrayLength++] = arr[index]->left;
                    }
                    if(arr[index]->right)
                    {
                        tempArray[tempArrayLength++] = arr[index]->right;
                    }
                    if(first == null)
                    {
                        first = arr[index];
                        first->right = null;
                        first->left = null;
                        last = first;
                    }
                    else
                    {
                        last->left = arr[index];
                        last->left->right = last;
                        last->left->left = null;
                        last = last->left;
                    }
                }
            }
            if(tempArrayLength > 0)
            {
                tempNodePtr = PressTree(tempArray,tempArrayLength,!straight);
            }
            if(last)
            {
                last->left = tempNodePtr;
            }
            if(tempNodePtr)
            {
                tempNodePtr->right = last;
            }

        }
        else
        {
            tempArrayLength = 0;
            //Populate Temp Array
            for(index=0;index<length;index++)
            {
                if(arr[index])
                {
                    if(arr[index]->left != null)
                    {
                        tempArray[tempArrayLength++] = arr[index]->left;
                    }
                    if(arr[index]->right != null)
                    {
                        tempArray[tempArrayLength++] = arr[index]->right;
                    }
                }
            }
            first = null;
            last = null;
            for(index=length-1;index>=0;index--)
            {
                if(arr[index])
                {
                    if(first == null)
                    {
                        first = arr[index];
                        first->right = null;
                        first->left = null;
                        last = first;
                    }
                    else
                    {
                        last->left = arr[index];
                        last->left->right = last;
                        last->left->left = null;
                        last = last->left;
                    }
                }
            }
            if(tempArrayLength > 0)
            {
                tempNodePtr = PressTree(tempArray,tempArrayLength,!straight);
            }
            if(last)
            {
                last->left = tempNodePtr;
            }
            if(tempNodePtr)
            {
                tempNodePtr->right = last;
            }
        }
        free(tempArray);
    }
    return first;
}


//UnSafe:..cyclic dependency not checked
TreeNodePtr FlattenTree(TreePtr tree)
{
    TreeNodePtr arr[1];
    if(tree->root)
    {
        arr[0]= tree->root;
        return PressTree(arr,1,0);
    }
    return null;
}


