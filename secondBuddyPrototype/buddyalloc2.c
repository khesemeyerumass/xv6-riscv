#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Prototype of The Buddy Memory Allocator

// Struct that represents the elements of the list of free memory
struct freeBlock {
    struct freeBlock *next; // Pointer to next free block of memory in the list
    unsigned int size; // size of block of memory
    void* ptr; // Pointer to the actual free space
};

// Struct representing each node of the binary tree that represents all used/unused memory blocks
struct treeNode {
    unsigned int size;
    bool isSplit;
    bool isUsed;
    struct treeNode *leftChild;
    struct treeNode *rightChild;
    struct treeNode *parent;
    struct treeNode *nextRoot; // Will only be used for root nodes
    struct freeBlock *b; // Pointer to the freeBlock used to represent this node in the free list
};

typedef struct freeBlock Block;
typedef struct treeNode Node;

// Pointer to the front of the free list
static Block *freeList = NULL;

// Pointer to the root of the binary tree
static Node *root = NULL;

// Function used to initialize the free list and binary tree data structures before first allocation
void binit() {
    // Create dummy freeBlock (with 0 size) that will always remain at the beginning of the list
    freeList = (Block*) malloc(sizeof(Block));
    freeList->size = 0;
    freeList->ptr = NULL;

    // Create freeBlock that is associated with this specifically-sized portion of memory
    Block *newB = (Block*) malloc(sizeof(Block));
    newB->size = 131072;
    newB->ptr = malloc(131072);
    freeList->next = newB;

    // Create binary tree node associated with this specifically-sized portion of memory
    root = (Node*) malloc(sizeof(Node));
    root->size = 131072; // 2 to the power of 16
    root->isSplit = false;
    root->isUsed = false;
    root->b = newB;
}

// Recursive function that finds the node in the binary tree associated with this memory segment
static Node* getNode(void* memAdd, Node* curr) {
    if (curr != NULL && memAdd != NULL) {
        // Check if the current node is desired one
        if (!curr->isSplit && curr->b->ptr == memAdd) {
            return curr;
        } else if (curr->isSplit){
            // Check left child
            Node *next = getNode(memAdd, curr->leftChild);
            if (next != NULL && !next->isSplit && next->b->ptr == memAdd) {
                return next;
            }

            // Check right child
            Node *next2 = getNode(memAdd, curr->rightChild);
            if (next2 != NULL && !next2->isSplit && next2->b->ptr == memAdd) {
                return next2;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

// Function that can be used to remove a specific block of memory from the free list
static void removeFromList(Block* block) {
    Block *curr, *prev;
    prev = freeList;
    for (curr = prev->next; curr != NULL; prev = curr, curr = curr->next) {
        if (curr == block) {
            prev->next = curr->next;
            curr->next = NULL;
            break;
        }
    }
}

// Function used to free memory (add it to the free list data structure + check for buddy)
void bfree(void* mem) {
    // First we need to determine the the node associated with this memory
    Node *currentRoot = root;
    Node *n = NULL;
    bool foundRoot = false;
    while (!foundRoot) {
        n = getNode(mem, currentRoot);
        if (n != NULL) {
            foundRoot = true;
        } else {
            currentRoot = currentRoot->nextRoot;
        }
    }

    if (n != NULL && mem == n->b->ptr) {
        n->isUsed = false;
        if (n->parent == NULL) {
            // Dealing with a root node
            n->b->next = freeList->next;
            freeList->next = n->b;
        } else {
            // Dealing with non-root node
            bool canMerge = true;
            Node *buddy = NULL;

            // Determine whether you can still merge current block with buddy block
            while (canMerge) {
                if (n->parent == NULL) {
                    canMerge = false;
                    continue;
                } else if (n->parent->leftChild != n) {
                    buddy = n->parent->leftChild;
                } else {
                    buddy = n->parent->rightChild;
                }

                // Check whether the memory can be merge with its buddy
                if (!buddy->isUsed && !buddy->isSplit) {
                    // First remove the buddy block from the free list
                    removeFromList(buddy->b);

                    // Update parent node and free the children blocks and nodes
                    // (since they are not needed anymore)
                    n = n->parent;
                    buddy = NULL;
                    n->isSplit = false;

                    Block *temp1 = n->leftChild->b;
                    temp1->ptr = NULL;
                    Block *temp2 = n->rightChild->b;
                    temp2->ptr = NULL;
                    Node *temp3 = n->leftChild;
                    temp3->b = NULL;
                    Node *temp4 = n->rightChild;
                    temp4->b = NULL;
                    n->leftChild = NULL;
                    n->rightChild = NULL;

                    free(temp1);
                    free(temp2);
                    free(temp3);
                    free(temp4);
                } else {
                    canMerge = false;
                }
            }

            // Add the currently freed and possibly merged memory back to the free list
            n->b->next = freeList->next;
            freeList->next = n->b;
        }
    }
}

void* balloc(unsigned int nBytes) {
    // Check whether data structure haven't been initialized
    if (root == NULL) {
        binit();
    }

    Block *curr, *prev;
    prev = freeList;

    Node *currRoot = root;
    for (curr = prev->next; curr != NULL; prev = curr, curr = curr->next) {
        if (curr->size == nBytes) {
            // Found block of memory that contains the exact amount requested
            prev->next = curr->next;
            curr->next = NULL;
            Node *n = getNode(curr->ptr, currRoot);
            n->isUsed = true;
            return curr->ptr;
        } else if (curr->size >= nBytes) {
            // Found acceptable block of memory, but need to check in case it can be split
            prev->next = curr->next;
            curr->next = NULL;
            Node *n = getNode(curr->ptr, currRoot);

            int currSize = curr->size;
            while ((currSize / 2) >= nBytes) {
                currSize = currSize / 2.0;
                // If it can be split, create the new block and its buddy
                Block *newB = (Block*) malloc(sizeof(Block));
                newB->size = currSize;
                newB->ptr = curr->ptr;

                Block *buddy = (Block*) malloc(sizeof(Block));
                buddy->size = currSize;
                buddy->ptr = curr->ptr + currSize;

                // Update info in current node and create two new nodes for binary tree
                n->isSplit = true;

                Node *left = (Node*) malloc(sizeof(Node));
                n->leftChild = left;
                left->size = currSize;
                left->parent = n;
                left->b = newB;

                Node *right = (Node*) malloc(sizeof(Node));
                n->rightChild = right;
                right->size = currSize;
                right->parent = n;
                right->b = buddy;

                // Add buddy to free list. Update current block and node
                buddy->next = freeList->next;
                freeList->next = buddy;

                curr = newB;
                n = left;
            }
            return curr->ptr;
        }

//        // If the current available memory isn't big enough, add more to the free list
//        if (curr->next == NULL) {
//            Block *newB = (Block*) malloc(sizeof(Block));
//            newB->size = 131072;
//            newB->ptr = (void*) malloc(131072);
//            curr->next = newB;
//
//            Node *newR = (Node*) malloc(sizeof(Node));
//            newR->size = 131072; // 2 to the power of 16
//            newR->isSplit = false;
//            newR->isUsed = false;
//            newR->b = newB;
//
//            while (currRoot->nextRoot != NULL) {
//                currRoot = currRoot->nextRoot;
//            }
//            currRoot->nextRoot = newR;
//        }
    }
    return NULL;
}


int main() {

    binit();
    if (freeList->next != NULL) {
        printf("Size of block: %u \n\n", freeList->next->size);
    }

    printf("Test1: Allocating 131072 bytes and then freeing it \n");
    void* temp = balloc(131072);
    if (freeList->next == NULL) {
        printf("Empty\n");
    }

    bfree(temp);

    if (freeList->next != NULL) {
        printf("Size of block: %u \n\n", freeList->next->size);
    }

    printf("Test2: Allocating 65536 bytes twice and then freeing them both\n");
    void* temp2 = balloc(65536);
    if (freeList->next != NULL) {
        printf("Size of block: %u \n", freeList->next->size);
    }
    if (root->isSplit) {
        printf("Size of left child block: %u \n", root->leftChild->size);
        printf("Size of right child block: %u \n", root->rightChild->size);
    }

    void* temp3 = balloc(65536);
    if (freeList->next == NULL) {
        printf("Empty\n");
    }

    bfree(temp2);
    if (freeList->next != NULL) {
        printf("Size of block: %u \n", freeList->next->size);
    }
    bfree(temp3);
    if (freeList->next != NULL) {
        printf("Size of block: %u \n\n", freeList->next->size);
    }

    printf("Test4: Allocating 32768 bytes, 65536 bytes, 32768 bytes, and then freeing them all\n");
    void *temp4 = balloc(32768);
    void *temp5 = balloc(65536);
    printf("Size of block: %u \n", freeList->next->size);
    void *temp6 = balloc(32768);
    if (freeList->next == NULL) {
        printf("Empty\n");
    } else {
        printf("Size of block: %u \n", freeList->next->size);
    }

    bfree(temp4);
    printf("Size of block: %u \n", freeList->next->size);
    bfree(temp5);
    printf("Size of block: %u \n", freeList->next->size);
    bfree(temp6);
    printf("Size of block: %u \n", freeList->next->size);

    if (freeList->next->next == NULL) {
        printf("Currently only 1 free block of memory\n");
    }

    if (root->rightChild != NULL) {
        printf("Fail\n");
    }

    void *temp7 = balloc(65036);
    printf("Size of block: %u \n", freeList->next->size);
//    void *temp8 = balloc(131072);
//    if (temp8 == NULL) {
//        printf("Empty\n");
//    }

    return 0;
}
