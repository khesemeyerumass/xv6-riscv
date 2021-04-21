#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Test1: Allocating 131072 bytes, then 65536 bytes, and lastly freeing it \n");
  void* temp = malloc(131072);
  if (temp == 0) {
      printf("Empty\n");
  } else {
      printf("Success\n");
  }
  void* temp1 = malloc(65536);
  if (temp1 == 0) {
      printf("Empty\n");
  } else {
      printf("Success\n");
  }
  free(temp);
  free(temp1);
  
  printf("Test2: Allocating 2147483648 bytes. Allocate any amount after and then use free.\n");
  void* temp3 = malloc(2147483648);
  if (temp3 == 0) {
      printf("Empty\n");
  } else {
      printf("Success\n");
  }
  void* temp4 = malloc(2147483648);
  if (temp4 == 0) {
      printf("Empty\n");
  } else {
      printf("Success\n");
  }
  void* temp5 = malloc(2147483648);
  if (temp5 == 0) {
      printf("Empty\n");
  } else {
      printf("Success\n");
  }
  free(temp3);
  free(temp4);
  free(temp5);
  
  exit(0);
}
