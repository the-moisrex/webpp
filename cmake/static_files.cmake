function(generate_config_file config_file)
  

  list(LENGTH ARGN files_length)
  set(all_files "constexpr std::array<std::array<std::string_view, 2>, ${files_length}> static_files{{\n")

  set(sep_files "")
  
  foreach(_file IN LISTS ARGN)
    
    # generate a random prefix for strings
    string(RANDOM LENGTH 16 ALPHABET abcdefghijklmnopqrstuvwxyz0123456789 random_prefix)

    # read the file
    file(READ "${_file}" content)

    # calculate the length of the file (in bytes)
    string(LENGTH "${content}" content_length)
    string(LENGTH "${_file}" filename_length)

    # append the string to the final file
    string(APPEND all_files "\t{std::string_view{R\"${random_prefix}(${_file})${random_prefix}\", ${filename_length}}, std::string_view{R\"${random_prefix}(${content})${random_prefix}\", ${content_length}}},\n")

    # append as a separate variable
    string(MAKE_C_IDENTIFIER "${_file}" var_ident)
    string(APPEND sep_files "constexpr std::string_view file_${var_ident}{R\"${random_prefix}(${content})${random_prefix}\", ${content_length}};\n")

  endforeach()

  string(APPEND all_files "\n}};\n\n")

  set(file_content "#include <string_view>\n#include <array>\n\n")
  string(APPEND file_content "${all_files}\n")
  string(APPEND file_content "${sep_files}")

  # write the file_content to the config.h.in
  file(WRITE "${config_file}" "${file_content}")
endfunction()


