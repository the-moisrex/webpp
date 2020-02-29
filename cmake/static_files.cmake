
set(current_dir "${CMAKE_CURRENT_LIST_DIR}")

function(generate_config_file config_file_path)
  

  list(LENGTH ARGN files_length)
  set(all_files "constexpr std::array<std::array<std::string_view, 2>, ${files_length}> static_files{{\n")

  set(sep_files "")
  
  foreach(_file IN LISTS ARGN)
    
    # generate a random prefix for strings
    string(RANDOM LENGTH 16 ALPHABET abcdefghijklmnopqrstuvwxyz0123456789 random_prefix)

    # read the file
    file(READ "${_file}" content)

    # configure the file to replace the variables
    #string(CONFIGURE "${_content}" content)

    # calculate the length of the file (in bytes)
    string(LENGTH "${content}" content_length)
    string(LENGTH "${_file}" filename_length)

    # append as a separate variable
    string(MAKE_C_IDENTIFIER "${_file}" var_ident)
    string(PREPEND var_ident "file_")
    string(APPEND sep_files "constexpr std::string_view ${var_ident}{R\"${random_prefix}(${content})${random_prefix}\", ${content_length}};\n")

    # append the string to the final file
    string(APPEND all_files "\t{std::string_view{R\"${random_prefix}(${_file})${random_prefix}\", ${filename_length}}, ${var_ident}},\n")


  endforeach()

  string(APPEND all_files "\n}};\n\n")

  string(TIMESTAMP datetime UTC)

  set(file_content "")
  string(APPEND file_content "// Auto generated with cmake at: ${datetime}\n// Written by: Mohammad Bahoosh\n\n")
  string(APPEND file_content "#include <string_view>\n#include <array>\n\n")
  string(APPEND file_content "${sep_files}\n\n")
  string(APPEND file_content "${all_files}\n\n")

  file(READ "${current_dir}/config_template.h.in" config_template)
  string(APPEND file_content "${config_template}\n")

  # write the file_content to the config.h.in
  file(WRITE "${config_file_path}" "${file_content}")

  # set the CONFIG_FILE variable
  set(CONFIG_FILE "${config_file_path}" PARENT_SCOPE)
endfunction()


