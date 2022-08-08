project(statekiro_console
  VERSION      1.0.0.0
  DESCRIPTION  "Console interface to statekiro"
)

add_library(${PROJECT_NAME} SHARED main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE 
    statekiro_lib
)