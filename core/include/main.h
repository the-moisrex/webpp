#ifndef WEBPP_MAIN_H
#define WEBPP_MAIN_H

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
}
#endif // __unix__

#endif // WEBPP_MAIN_H
