#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Implementation of a user-level buddy allocator

// Union structure that will be used to represent a block of memory.
// It will also serve as elements of a linked list of free memory.
union block {
    struct {
        unsigned int size; // size of the memory block (will always be 4096 bytes)
        int numID; // number that represents this memory block
        int parentID; // number that represents this memory block's parent (if it is not a root)
        int buddyID; // number that represents this memory block's buddy block (if it is not a root)
        bool isSplit;
        bool isRoot; // used to indicate whether the block is a root or child
        union block *nextRoot; // will be NULL for child blocks
        union block *leftC;
        union block *rightC;
    };
};

typedef union block Block;

// pointer pointing to the first block of the free list (or NULL if empty)
static Block *freeList;

// will be used when giving number IDs to each memory block
static int count = 0;

void bfree(void *freedMem) {
    Block *newBlock, *current;
    newBlock = (Block*) freedMem - 1;
    if (newBlock->isRoot) {
        // Add root block of memory to the end of the list
        for (current = freeList; current->nextRoot != NULL; current = current->nextRoot);
        current->nextRoot = newBlock;
    } else {
        // Todo Freeing child block (check for buddy blocks)
    }
}

// Function used to add more memory to the free list of memory (root blocks)
static Block* addmemory(unsigned int nBytes) {
    char *temp;
    Block *newRoot;

    // In case freelist has not been constructed yet
    if (freeList == NULL) {
        freeList = (Block*) malloc(sizeof(Block));
        freeList->size = 0;
        freeList->isSplit = false;
        freeList->isRoot = true;
        freeList->numID = count;
        count = count + 1;
    }

    // Ensure that the requested size of memory is 4096 bytes
    if (nBytes != 4096) {
        nBytes = 4096;
    }

    // Allocate 4096 bytes + additional bytes for Root structure
    temp = (char*) malloc(nBytes + sizeof(Block));
    if (temp == NULL) {
        return NULL;
    }
    newRoot = (Block*) temp;

    // Properly initialize certain variables of the new root block
    newRoot->size = nBytes;
    newRoot->numID = count;
    newRoot->isSplit = false;
    newRoot->isRoot = true;
    count = count + 1;

    // Add the new root to the free list of memory using the free function
    bfree((void*)(newRoot + 1));
    return freeList;
}

void* balloc(unsigned int nBytes) {
    Block *current, *previous;

    if (freeList == NULL) {
        freeList = (Block*) malloc(sizeof(Block));
        freeList->size = 0;
        freeList->isSplit = false;
        freeList->isRoot = true;
        freeList->numID = count;
        count = count + 1;
    }

    previous = freeList;
    for (current = previous->nextRoot; ; previous = current, current = current->nextRoot) {
        if (current == NULL) {
            addmemory(nBytes);
            current = previous->nextRoot;
        }

        if (current->size >= nBytes) {
            if (current->size == nBytes || (current->size / 2.0) < nBytes) {
                previous->nextRoot = current->nextRoot;
                return (void*) (current + 1);
            } else {
                // Todo: splitting
                int newSize = current->size / 2.0;
                while (newSize >= nBytes) {

                }
            }
        }
    }
}

// Test for just seeing that you can add root blocks, allocate them, and then free
// the allocated roots. Also to see whether freed blocks of memory still contained their
// union data.
int main() {
    addmemory(4096);

    if (freeList->nextRoot != NULL) {
        printf("Size of block: %u \n", freeList->nextRoot->size);
    }

    void* temp = balloc(4096);

    if (freeList->nextRoot == NULL) {
        printf("Empty\n");
    }

    bfree(temp);

    if (freeList->nextRoot != NULL) {
        printf("Size of block: %u \n", freeList->nextRoot->size);
    }

    return 0;
}
