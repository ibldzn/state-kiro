project(statekiro_lib
  VERSION      1.0.0.0
  DESCRIPTION  "Stream proof state indicator for sekiro (LIB)"
)

if(MSVC)
  enable_language(ASM_MASM)
endif()

include(FetchContent)

FetchContent_Declare(minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook
  GIT_TAG        origin/master
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(minhook)

add_library(${PROJECT_NAME} STATIC
  src/statekiro.cpp

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

target_include_directories(${PROJECT_NAME}
  PRIVATE
    src
    include/statekiro
  INTERFACE
    include
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE 
    dxguid
    minhook
)