#ifndef WEBPP_MAIN_HPP
#define WEBPP_MAIN_HPP

#ifdef __unix__
extern "C" {
// details:
// https://polentino911.wordpress.com/2013/08/08/make-your-own-executable-shared-library-on-linux/
#    ifdef ELF_INTERPRETER_PATH
extern const char elf_interpreter[] __attribute__((section(".interp"))) =
  ELF_INTERPRETER_PATH;
#    else  // ELF_INTERPRETER_PATH
extern const char elf_interpreter[] __attribute__((section(".interp"))) =
  "/lib64/ld-linux-x86-64.so.2";
#    endif // ELF_INTERPRETER_PATH
extern void __lib_main(void) __attribute__((noreturn));



#    include <stdio.h>
#    include <stdlib.h>

/**
 * This function is used for printing information about the project
 * when running the dynamic library.
 *
 * Even though we could do some really amazing stuff here but we're not
 * gonna get very clever about it just yet because we don't want to
 * make the library heavy.
 */
void __lib_main(void) {
    printf("web++ framework in the flesh.\n");
    exit(0);
}
}
#endif
#endif // WEBPP_MAIN_HPP
