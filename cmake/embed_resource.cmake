# https://gitlab.com/jhamberg/cmake-examples/-/blob/master/cmake/FileEmbed.cmake
set(EMBED_TARGET embedded_resources)
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
function(embed_setup custom_dependencies)
  if (NOT EXISTS ${GENERATED_DIR})
    file(MAKE_DIRECTORY ${GENERATED_DIR})
  endif()
  if (NOT EXISTS ${GENERATED_DIR}/empty.h)
    file(WRITE ${GENERATED_DIR}/empty.h "")
  endif()
  add_custom_target(${EMBED_TARGET}
    DEPENDS $<TARGET_PROPERTY:${EMBED_TARGET},INSTALLED_FILES> ${custom_dependencies})
endfunction()

function(embed_resource resource_file_name source_file_name variable_name)
  if(EXISTS "${source_file_name}")
    if("${source_file_name}" IS_NEWER_THAN "${resource_file_name}")
      return()
    endif()
  endif()

  file(READ "${resource_file_name}" hex_content HEX)

  string(REPEAT "[0-9a-f]" 32 pattern)
  string(REGEX REPLACE "(${pattern})" "\\1\n  " content "${hex_content}")

  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

  string(REGEX REPLACE ", $" "" content "${content}")

  set(array_definition "static const unsigned char ${variable_name}[] = {\n  ${content}\n};")
  set(size_definition "static const unsigned ${variable_name}_size = sizeof(${variable_name});")
  set(source "// Auto generated file.\n${array_definition}\n${size_definition}\n")

  file(WRITE "${source_file_name}" "${source}")
endfunction()

function(embed_resource_add resource_file_name source_file_name variable_name)
  embed_resource(${resource_file_name} ${source_file_name} ${variable_name})
  add_custom_command(
    OUTPUT ${source_file_name}
    COMMAND ${CMAKE_COMMAND}
    -DRUN_EMBED_GENERATE=1
    -DFILE_RFN=${resource_file_name}
    -DFILE_SFN=${source_file_name}
    -DFILE_VN=${variable_name}
    -P ${CMAKE_SOURCE_DIR}/cmake/embed_resource.cmake
    DEPENDS ${resource_file_name}
  )
  # https://discourse.cmake.org/t/is-there-depends-property-for-custom-targets/4207/4
  set_property(TARGET ${EMBED_TARGET} APPEND PROPERTY
    INSTALLED_FILES ${source_file_name}
  )
endfunction()

if (RUN_EMBED_GENERATE)
  embed_resource(${FILE_RFN} ${FILE_SFN} ${FILE_VN})
endif()