#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // This program was designed for measuring the time it takes for either memory 
  // allocator to operate in xv6.
  if (argc == 2) {
    void** temps[atoi(argv[1])];
    for (int i = 0; i < atoi(argv[1]); i++) {
      temps[i] = malloc(100000);
    }
    for (int i = 0; i < atoi(argv[1]); i++) {
      free(temps[i]);
    }
    printf("lol\n");
  }
  exit(0);
}
