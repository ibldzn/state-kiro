#include <imgui/imgui.h>
#include <render/views/views.hpp>
#include <statekiro/statekiro.hpp>

void ViewPostureBar::on_init()
{
}

void ViewPostureBar::draw()
{
    if (!m_show_posture_bar) {
        return;
    }

    ImGui::Begin("##posture_bar", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const float normalized_posture = [] {
        const unsigned int max_posture = statekiro::get_max_posture();
        if (max_posture == 0) {
            return 1.f;
        }
        return static_cast<float>(statekiro::get_current_posture()) / static_cast<float>(max_posture);
    }();

    ImGui::ProgressBar(1.f - normalized_posture, ImVec2(0, 0), "");
    ImGui::End();
}