#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

//#define DEFAULTALLOC
#define BUDDYALLOC

#ifdef BUDDYALLOC
    // Struct for nodes of the binary tree that represents all used/unused memory blocks
    struct treeNode {
        unsigned int size;
        int isSplit;  // 0 = false, 1 = true
        int isUsed;   // 0 = false, 1 = true
        struct treeNode *leftChild;
        struct treeNode *rightChild;
        struct treeNode *parent;
        struct treeNode *nextRoot; // Will only be used for root nodes
        void* mem; // Pointer to the actual free space
    };

    typedef struct treeNode Node;

    // Pointer to the root of the binary tree
    static Node *root = 0;

    // Function used to initialize binary tree data structure before first allocation
    void binit() {
        // Create binary tree node associated with this specifically-sized portion of memory
        root = (Node*) sbrk(4096 * sizeof(Node));
        root->size = 131072; // 2 to the power of 16
        root->isSplit = 0;
        root->isUsed = 0;
        root->mem = (void*) sbrk(131072);
    }

    // Recursive function that finds the node in the binary tree associated with this memory segment
    static Node* getNode(void* memAdd, Node* curr) {
        if (curr != 0 && memAdd != 0) {
            // Check if the current node is desired one
            if (curr->isSplit == 0 && curr->mem == memAdd) {
                return curr;
            } else if (curr->isSplit == 1){
                // Check left child
                Node *next = getNode(memAdd, curr->leftChild);
                if (next != 0 && next->isSplit == 0 && next->mem == memAdd) {
                    return next;
                }

                // Check right child
                Node *next2 = getNode(memAdd, curr->rightChild);
                if (next2 != 0 && next2->isSplit == 0 && next2->mem == memAdd) {
                    return next2;
                }
            } else {
                return 0;
            }
        }
        return 0;
    }
    
    // Recursive function that finds any node representing free memory and is big enough
    static Node* findAvailableMem(Node* curr, unsigned int size) {
        if (curr != 0) {
            // Check if the current node is desired one
            if (curr->isSplit == 0 && curr->isUsed == 0 && curr->size >= size) {
                return curr;
            } else if (curr->isSplit == 1){
                // Check left child
                Node *next = findAvailableMem(curr->leftChild, size);
                if (next != 0 && next->isSplit == 0 && next->isUsed == 0 && next->size >= size) {
                    return next;
                }

                // Check right child
                Node *next2 = findAvailableMem(curr->rightChild, size);
                if (next2 != 0 && next2->isSplit == 0 && next2->isUsed == 0 && next2->size >= size) {
                    return next2;
                }
            } else {
                return 0;
            }
        }
        return 0;
    }
#endif

#ifdef DEFAULTALLOC
    typedef long Align;

    union header {
      struct {
        union header *ptr;
        uint size;
      } s;
      Align x;
    };

    typedef union header Header;

    static Header base;
    static Header *freep;
#endif

void
free(void *ap)
{
  #ifdef DEFAULTALLOC
      Header *bp, *p;

      bp = (Header*)ap - 1;
      for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
          break;
      if(bp + bp->s.size == p->s.ptr){
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
      } else
        bp->s.ptr = p->s.ptr;
      if(p + p->s.size == bp){
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
      } else
        p->s.ptr = bp;
      freep = p;
  #endif
  #ifdef BUDDYALLOC
      // First we need to determine the the node associated with this memory
      Node *currentRoot = root;
      Node *n = 0;
      int foundRoot = 0;
      while (foundRoot == 0) {
          n = getNode(ap, currentRoot);
          if (n != 0) {
              foundRoot = 1;
          } else {
              currentRoot = currentRoot->nextRoot;
          }
      }

      if (n != 0 && ap == n->mem) {
          n->isUsed = 0;
          if (n->parent != 0) {
              // Dealing with a non-root node
              int canMerge = 1;
              Node *buddy = 0;

              // Determine whether you can still merge current block with buddy block
              while (canMerge == 1) {
                  if (n->parent == 0) {
                      // Reached the root node
                      canMerge = 0;
                      continue;
                  } else if (n->parent->leftChild != n) {
                      buddy = n->parent->leftChild;
                  } else {
                      buddy = n->parent->rightChild;
                  }

                  // Check whether the memory can be merge with its buddy
                  if (buddy->isUsed == 0 && buddy->isSplit == 0) {
                      // Update parent node and get rid of its children
                      n = n->parent;
                      buddy = 0;
                      n->isSplit = 0;

                      n->leftChild = 0;
                      n->rightChild = 0;
                  } else {
                      canMerge = 0;
                  }
              }
          }
      }
  #endif
}

#ifdef DEFAULTALLOC
    static Header*
    morecore(uint nu)
    {
      char *p;
      Header *hp;

      if(nu < 4096)
        nu = 4096;
      p = sbrk(nu * sizeof(Header));
      if(p == (char*)-1)
        return 0;
      hp = (Header*)p;
      hp->s.size = nu;
      free((void*)(hp + 1));
      return freep;
    }
#endif

void*
malloc(uint nbytes)
{
  #ifdef DEFAULTALLOC
      Header *p, *prevp;
      uint nunits;

      nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
      if((prevp = freep) == 0){
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
      }
      for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
        if(p->s.size >= nunits){
          if(p->s.size == nunits)
            prevp->s.ptr = p->s.ptr;
          else {
            p->s.size -= nunits;
            p += p->s.size;
            p->s.size = nunits;
          }
          freep = prevp;
          return (void*)(p + 1);
        }
        if(p == freep)
          if((p = morecore(nunits)) == 0)
            return 0;
      }
  #endif 
  #ifdef BUDDYALLOC
      // Check whether data structure haven't been initialized
      if (root == 0) {
          binit();
      }
    
      // Search the tree for an available memory block that is of valid size
      Node* curr = findAvailableMem(root, nbytes);
    
      if (curr == 0) {
          return 0;
      } else {
          // Need to check if the block of free memory is large enough to be split
          int size = curr->size;
          while ((size / 2.0) >= nbytes) {
              size = size / 2.0;
            
              // Update info in current node and create two new child nodes for binary tree
              curr->isSplit = 1;

              Node *left = (Node*) sbrk(4096 * sizeof(Node));
              curr->leftChild = left;
              left->size = size;
              left->parent = curr;
              left->mem = curr->mem;
              left->isSplit = 0;
              left->isUsed = 0;

              Node *right = (Node*) sbrk(4096 * sizeof(Node));
              curr->rightChild = right;
              right->size = size;
              right->parent = curr;
              right->mem = curr->mem + size;
              right->isSplit = 0;
              right->isUsed = 0;
            
            
              curr = left;
          }
          curr->isUsed = 1;
          return curr->mem;
      }
  #endif
}
