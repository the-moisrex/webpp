#ifdef __unix__

#include "../include/main.h"
#include <stdio.h>
#include <stdlib.h>

extern "C" void __lib_main(void) {
  printf("web++ framework in the flesh.\n");
  exit(0);
}

#endif
