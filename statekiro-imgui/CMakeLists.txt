project(statekiro_imgui
  VERSION      1.0.0.0
  DESCRIPTION  "ImGui interface to statekiro"
)

add_subdirectory(libs)

add_library(${PROJECT_NAME} SHARED
  src/main.cpp

  src/render/input.cpp
  src/render/render.cpp
  src/render/views/views.cpp
  src/render/views/view_config.cpp
  src/render/views/view_health_bar.cpp
  src/render/views/view_posture_bar.cpp
  src/render/views/view_qitem_prosthetic.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    imgui
    statekiro_lib
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    src
)