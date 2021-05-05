#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Program that can be used to record information regarding the internal
// fragmentation of the memory allocator. This is only intended for
// the buddy allocator since the storage allocator always allocates the 
// exact amount of memory requested (meaning it always has 0 internal
// fragmentation)

int
main(int argc, char *argv[])
{
  // First allocation request will be for 50000 bytes. Each successive allocation request
  // will increase by an additional 5000 bytes and 2500 allocations will be done in the
  // end. The very last allocation request will be for 12545000 bytes.
  int x = 50000;
  int p = 2;
  for (int i = 0; i < 150; i += 10) {
    void* temp = malloc(x + (i * 5000));
    while (p < x + (i * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + (i * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + (i * 5000)));
    
    void* temp1 = malloc(x + ((i+1) * 5000));
    while (p < x + ((i+1) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+1) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+1) * 5000)));
    
    void* temp2 = malloc(x + ((i+2) * 5000));
    while (p < x + ((i+2) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+2) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+2) * 5000)));
    
    void* temp3 = malloc(x + ((i+3) * 5000));
    while (p < x + ((i+3) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+3) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+3) * 5000)));
    
    void* temp4 = malloc(x + ((i+4) * 5000));
    while (p < x + ((i+4) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+4) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+4) * 5000)));
    
    void* temp5 = malloc(x + ((i+5) * 5000));
    while (p < x + ((i+5) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+5) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+5) * 5000)));
    
    void* temp6 = malloc(x + ((i+6) * 5000));
    while (p < x + ((i+6) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+6) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+6) * 5000)));
    
    void* temp7 = malloc(x + ((i+7) * 5000));
    while (p < x + ((i+7) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+7) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+7) * 5000)));
    
    void* temp8 = malloc(x + ((i+8) * 5000));
    while (p < x + ((i+8) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+8) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+8) * 5000)));
    
    void* temp9 = malloc(x + ((i+9) * 5000));
    while (p < x + ((i+9) * 5000)) {
      p = p * 2;
    }
    printf("Size of request: %d\n", x + ((i+9) * 5000));
    printf("Amount of memory allocated: %d\n", p);
    printf("Amount of internal fragmentation: %d\n", p - (x + ((i+9) * 5000)));
  
    // Free allocated memory
    free(temp);
    free(temp1);
    free(temp2);
    free(temp3);
    free(temp4);
    free(temp5);
    free(temp6);
    free(temp7);
    free(temp8);
    free(temp9);
  }
  exit(0);
}
