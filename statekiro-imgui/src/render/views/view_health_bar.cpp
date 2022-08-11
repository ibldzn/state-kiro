#include <imgui/imgui.h>
#include <render/views/views.hpp>
#include <statekiro/statekiro.hpp>

void ViewHealthBar::on_init()
{
}

void ViewHealthBar::draw()
{
    if (!m_show_health_bar) {
        return;
    }

    ImGui::Begin("##health_bar", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const float normalized_health = [] {
        const unsigned int max_health = statekiro::get_max_health();
        if (max_health == 0) {
            return 1.f;
        }
        return static_cast<float>(statekiro::get_current_health()) / static_cast<float>(max_health);
    }();

    ImGui::ProgressBar(normalized_health, ImVec2(0, 0), "");
    ImGui::End();
}