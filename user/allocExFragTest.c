#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc == 2 && atoi(argv[1]) == 1) {
    // Buddy Allocator Test
    int x = 600;
    int memAllocated = 1024;
    int totalMem = 300000;
    for (int i = 0; i < 270; i++) {
      void* temp = malloc(x);
      printf("Size of request: %d\n", x);
      totalMem = totalMem - memAllocated;
      printf("Amount of external fragmentation: %d\n", totalMem);
      
      // Need to use pointer in some manner or else xv6 gives me an error
      if (temp == 0) {
        printf("Error");
      }
    }
  } else {
    // Storage Allocator Test
    int x = 600;
    int totalMem = 300000;
    
    // First malloc and free 300000 bytes (so that free list has 300000 bytes at start of experiment)
    void* t = malloc(300000);
    free(t);
    
    for (int i = 0; i < 270; i++) {
      void* temp = malloc(x);
      printf("Size of request: %d\n", x);
      totalMem = totalMem - x;
      printf("Amount of external fragmentation: %d\n", totalMem);
    
      // Need to use pointer in some manner or else xv6 gives me an error
      if (temp == 0) {
        printf("Error");
      }
    }
  } 
  exit(0);
}
