project(imgui)

add_library(imgui STATIC
  src/imgui.cpp
  src/imgui_demo.cpp
  src/imgui_draw.cpp
  src/imgui_tables.cpp
  src/imgui_widgets.cpp
  src/imgui_stdlib.cpp
  src/imgui_impl_dx11.cpp
  src/imgui_impl_win32.cpp
)

add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui
  PRIVATE
    include/imgui
  INTERFACE
    include
)
