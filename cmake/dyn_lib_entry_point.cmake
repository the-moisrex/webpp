
# Nastiest way possible to make a shared library (.so file) executable
# It only works in UNIX world.

function(set_interp_path)

  set(INTERPRETER_DESCRIPTION "Requesting program interpreter:")
   
  execute_process(COMMAND ${CMAKE_READELF} -l /lib/libc.so.6
      RESULT_VARIABLE return_value
      OUTPUT_VARIABLE result
  )
   
  if(return_value)
      message(WARNING "Cannot find a valid ELF interpreter. You will not be able to run the shared library as a normal executable. You don't need to worry about this if you don't need this feature.")
  else()
      string(REGEX REPLACE
          ".*[[]${INTERPRETER_DESCRIPTION} ([/][^ ].+)[]].*" "\\1"
          _ELF_INTERPRETER_PATH "${result}"
      )
   
      add_definitions(-DELF_INTERPRETER_PATH="${_ELF_INTERPRETER_PATH}" )
      message(STATUS "ELF interpreter                : ${_ELF_INTERPRETER_PATH}")
  endif() 

endfunction()

