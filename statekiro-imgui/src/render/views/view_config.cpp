#include <imgui/imgui.h>
#include <render/views/views.hpp>

void ViewConfig::on_init()
{
    RegisterHotkey(0x70 /*VK_F1*/, [] { m_show_config_window = !m_show_config_window; });
}

void ViewConfig::draw()
{
    if (auto& io = ImGui::GetIO(); m_show_config_window) {
        io.MouseDrawCursor = true;
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    } else {
        io.MouseDrawCursor = false;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
        return;
    }

    ImGui::Begin("##config_window");

    ImGui::Checkbox("Show health bar", &m_show_health_bar);
    ImGui::Checkbox("Show posture bar", &m_show_posture_bar);
    ImGui::Checkbox("Show quick item & prosthetic", &m_show_quick_item_prosthetic);

    ImGui::End();
}