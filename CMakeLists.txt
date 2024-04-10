cmake_minimum_required(VERSION 3.18)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg (only Windows)
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
  message(STATUS "Target triplet for debug is " ${VCPKG_TARGET_TRIPLET})
else()
  set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
  message(STATUS "Target triplet for release is " ${VCPKG_TARGET_TRIPLET})
endif()

project(cocaine)

#
# CEF Configuration
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CEF_VERSION "110.0.31+g46651e0+chromium-110.0.5481.179")
set(CEF_PLATFORM "windows64")
include(DownloadCEF)
include(embed_resource)
DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_SOURCE_DIR}/thirdparty/cef")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(hidapi CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(CEF REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(murmurhash CONFIG REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.hh" "src/*.h")
file(GLOB_RECURSE INCLUDES CONFIGURE_DEPENDS "include/*.cpp" "include/*.c" "include/*.hpp" "include/*.hh" "include/*.h")

# build_flatbuffers(
#   "src/config/config.fbs"
#   "${CMAKE_CURRENT_SOURCE_DIR}"
#   "schema"
#   ""
#   "${CMAKE_CURRENT_BINARY_DIR}/generated"
#   "${CMAKE_CURRENT_BINARY_DIR}/generated"
#   ""
# )

if (MSVC) # msvc + clang-cl/intel
  if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC") # clang-cl/intel only
    list(REMOVE_ITEM CEF_COMPILER_FLAGS 
      /MP           # MP = multiprocess compilation, clang-cl does not support MP flag
      /WX           # WX = treat warnings as errors, clang-cl causes warnings to be raised on things msvc won't?
    )
  endif()
endif()

PRINT_CEF_CONFIG()

add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
add_executable(cocaine ${SOURCES} ${INCLUDES})
SET_EXECUTABLE_TARGET_PROPERTIES(cocaine)
COPY_FILES(cocaine "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/bin")
COPY_FILES(cocaine "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/bin")
COPY_FILES(cocaine "tex.frag;tex.vert" "${CMAKE_CURRENT_SOURCE_DIR}/src/assets/shaders" "${CMAKE_CURRENT_BINARY_DIR}/bin/shaders")

add_dependencies(cocaine libcef_dll_wrapper)
if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  target_link_libraries(cocaine cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()
set_target_properties(cocaine PROPERTIES 
  CXX_STANDARD 20
  C_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_options(cocaine PUBLIC /EHsc)

embed_setup("")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/ares.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_ares.h" "IMG_ARES")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/bucky.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_bucky.h" "IMG_BUCKY")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/bulldog.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_bulldog.h" "IMG_BULLDOG")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/classic.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_classic.h" "IMG_CLASSIC")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/frenzy.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_frenzy.h" "IMG_FRENZY")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/ghost.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_ghost.h" "IMG_GHOST")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/guardian.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_guardian.h" "IMG_GUARDIAN")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/judge.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_judge.h" "IMG_JUDGE")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/marshal.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_marshal.h" "IMG_MARSHAL")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/melee.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_melee.h" "IMG_MELEE")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/odin.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_odin.h" "IMG_ODIN")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/operator.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_operator.h" "IMG_OPERATOR")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/phantom.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_phantom.h" "IMG_PHANTOM")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/sheriff.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_sheriff.h" "IMG_SHERIFF")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/shorty.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_shorty.h" "IMG_SHORTY")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/spectre.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_spectre.h" "IMG_SPECTRE")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/stinger.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_stinger.h" "IMG_STINGER")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/img/vandal.png" "${CMAKE_CURRENT_BINARY_DIR}/generated/img_vandal.h" "IMG_VANDAL")
embed_resource_add("${CMAKE_CURRENT_SOURCE_DIR}/src/assets/font/sp7.ttf" "${CMAKE_CURRENT_BINARY_DIR}/generated/font_sp7.h" "FONT_SP7")

add_compile_definitions(cocaine IMGUI_USER_CONFIG=<imconfig_user.h>)
add_dependencies(cocaine ${EMBED_TARGET})

target_include_directories(cocaine SYSTEM PRIVATE phnt/include)
target_include_directories(cocaine PRIVATE 
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/imgui
  ${CMAKE_SOURCE_DIR}/include/imgui/backends
  ${CMAKE_SOURCE_DIR}/include/imgui/misc/freetype
  ${CMAKE_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_BINARY_DIR})
target_link_directories(cocaine PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(cocaine
  nlohmann_json::nlohmann_json
  hidapi::hidapi hidapi::winapi hidapi::include
  GLEW::GLEW Freetype::Freetype
  murmurhash::murmurhash xxHash::xxhash
  opengl32 glfw3
  ntdll Bcrypt
  libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
# add_dependencies(soft schema)
