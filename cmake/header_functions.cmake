
# This function will links other extensions to their corresponding header
function(install_headers INPUT_DIR OUTPUT_DIR ALL_EXTENSIONS)
  message(STATUS "Header source files     : ${INPUT_DIR}")
  message(STATUS "Installing headers into : ${OUTPUT_DIR}")

  # copy .h extensions
  file(GLOB INCLUDE_ROOT_FILES_AND_DIRS 
    LIST_DIRECTORIES true 
    RELATIVE "${INPUT_DIR}"
    "${INPUT_DIR}/*")
  foreach(F IN LISTS INCLUDE_ROOT_FILES_AND_DIRS)
    file(INSTALL "${INPUT_DIR}/${F}" DESTINATION "${OUTPUT_DIR}")
  endforeach()

  # get the file names
  set(LIB_FILES "")
  foreach(F IN LISTS INCLUDE_ROOT_FILES_AND_DIRS)
    if (IS_DIRECTORY "${OUTPUT_DIR}/${F}")
      file(
        GLOB_RECURSE LIB_FILES__ 
        LIST_DIRECTORIES false
        "${OUTPUT_DIR}/${F}/*.hpp"
      )
      foreach(FF IN LISTS LIB_FILES__)
        if(NOT IS_DIRECTORY "${FF}")
          list(APPEND LIB_FILES "${FF}")
        endif()
      endforeach()
    else()
      list(APPEND LIB_FILES "${OUTPUT_DIR}/${F}")
    endif()
  endforeach()
  message(STATUS "Founded files with .h extension: ${LIB_FILES}")


  foreach(NEW_EXT IN LISTS ALL_EXTENSIONS)
    message(STATUS "Linking header files to their corresponding \"${NEW_EXT}\" extension.")
    foreach(F IN LISTS LIB_FILES)
      string(REPLACE ".hpp" "${NEW_EXT}" NEW_FILE "${F}")
      if(NOT IS_DIRECTORY "${NEW_FILE}")
        message(STATUS "Linking file \"${F}\" to \"${NEW_FILE}\"")
        file(CREATE_LINK "${F}" "${NEW_FILE}" COPY_ON_ERROR)
      endif()
    endforeach()
  endforeach()

endfunction()

