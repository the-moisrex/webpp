#ifdef __unix__

#include "../include/main.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * This function is used for printing information about the project
 * when running the dynamic library.
 *
 * Even though we could do some really amazing stuff here but we're not
 * gonna get very clever about it just yet because we don't want to
 * make the library heavy.
 */
extern "C" void __lib_main(void) {
  printf("web++ framework in the flesh.\n");
  exit(0);
}

#endif
