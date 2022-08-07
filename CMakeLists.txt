cmake_minimum_required(VERSION 3.22)

project(state_kiro
  VERSION      1.0.0.0
  DESCRIPTION  "Stream proof state indicator for sekiro"
)

if(MSVC)
  enable_language(ASM_MASM)
endif()

option(ENABLE_LTO "Enable Link Time Optimization" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type (Debug;Release;RelWithDebInfo;MinSizeRel)")
  message(NOTICE "No build type specified, defaulting to '${CMAKE_BUILD_TYPE}' build.")
endif()

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        9.0.0
  GIT_SHALLOW    TRUE
)

FetchContent_Declare(minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook
  GIT_TAG        origin/master
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(fmt minhook)

add_library(${PROJECT_NAME} SHARED
  src/main.cpp

  src/utils/utils.cpp

  src/hooks/hooks.cpp
  src/hooks/detour.cpp
  src/hooks/functions/get_current_health.cpp
  src/hooks/functions/get_current_posture.cpp
  src/hooks/functions/get_max_health.cpp
  src/hooks/functions/get_max_posture.cpp
  src/hooks/functions/present.cpp
  $<$<CXX_COMPILER_ID:MSVC>:src/hooks/functions/read_r10.asm>
  src/hooks/functions/sub_140E302B0.cpp

)

set_target_properties(${PROJECT_NAME} PROPERTIES
  USE_FOLDERS               ON
  CXX_EXTENSIONS            OFF
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/_out
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
  dxguid
  minhook
  fmt::fmt
)

if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT LTO_AVAILABLE OUTPUT LTO_ERROR)

  if(LTO_AVAILABLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
      INTERPROCEDURAL_OPTIMIZATION_RELEASE         ON
      INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO  ON
    )
  else()
    message(FATAL_ERROR "Failed to enable LTO!\n${LTO_ERROR}")
  endif()
endif()

if(WIN32)
  string(TIMESTAMP CURRENT_YEAR "%Y")
  set(USERNAME $ENV{USERNAME} CACHE STRING "Username")
  configure_file(src/version.rc.in ${CMAKE_BINARY_DIR}/version.rc @ONLY)
  target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
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
