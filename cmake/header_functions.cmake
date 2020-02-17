
# This function will links other extensions to their corresponding header
function(link_extensions INPUT_DIR OUTPUT_DIR INCLUDE_DIRNAME ALL_EXTENSIONS)

  # copy .h extensions
  file(INSTALL "${INPUT_DIR}" DESTINATION "${OUTPUT_DIR}")

  # get the file names
  file(
    GLOB_RECURSE LIB_FILES "${OUTPUT_DIR}/${INCLUDE_DIRNAME}"
    LIST_DIRECTORIES false
    *.h
  )
  message(STATUS "Founded files with .h extension: ${LIB_FILES}")


  foreach(NEW_EXT IN LISTS ALL_EXTENSIONS)
    message(STATUS "Linking header files to their corresponding \"${NEW_EXT}\" extension.")
    foreach(F IN LISTS LIB_FILES)
      string(REPLACE ".h" "${NEW_EXT}" NEW_FILE "${F}")
      if(NOT IS_DIRECTORY "${NEW_FILE}")
        file(CREATE_LINK "${F}" "${NEW_FILE}" COPY_ON_ERROR)
      endif()
    endforeach()
  endforeach()

endfunction()

