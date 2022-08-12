project(statekiro_loader
  VERSION      1.0.0.0
  DESCRIPTION  "Statekiro loader"
)

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY  https://github.com/fmtlib/fmt
  GIT_TAG         9.0.0
  GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(fmt)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    fmt::fmt
)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)