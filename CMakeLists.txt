cmake_minimum_required(VERSION 3.22)

project(statekiro)

option(ENABLE_LTO "Enable Link Time Optimization" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_out)

add_subdirectory(statekiro-lib)
add_subdirectory(statekiro-imgui)
add_subdirectory(statekiro-loader)
add_subdirectory(statekiro-console)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type (Debug;Release;RelWithDebInfo;MinSizeRel)")
  message(NOTICE "No build type specified, defaulting to '${CMAKE_BUILD_TYPE}' build.")
endif()

# Note:
# https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
# This option is implemented only by Makefile Generators and the Ninja. It is ignored on other generators.
if(CMAKE_GENERATOR MATCHES "(Ninja|Makefiles)")
  message(STATUS "Copying compile_commands.json..")
  # https://stackoverflow.com/a/60910583
  # Copy to source directory
  add_custom_target(
    copy-compile-commands ALL
    DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
  )
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    DEPENDS
      # Unlike "proper" targets like executables and libraries,
      # custom command / target pairs will not set up source
      # file dependencies, so we need to list file explicitly here
      generate-compile-commands
      ${CMAKE_BINARY_DIR}/compile_commands.json
  )

  # Generate the compilation commands. Necessary so cmake knows where it came
  # from and if for some reason you delete it.
  add_custom_target(generate-compile-commands
    DEPENDS
      ${CMAKE_BINARY_DIR}/compile_commands.json
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -B${CMAKE_BINARY_DIR} -S${CMAKE_SOURCE_DIR}
  )
endif()
