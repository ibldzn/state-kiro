#include <imgui/imgui.h>
#include <render/views/views.hpp>
#include <statekiro/statekiro.hpp>

void ViewQItemProsthetic::on_init()
{
}

void ViewQItemProsthetic::draw()
{
    if (!m_show_quick_item_prosthetic) {
        return;
    }

    ImGui::Begin("##quick_item_prosthetic", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    ImGui::TextUnformatted(statekiro::get_current_quick_item().c_str());

    ImGui::SameLine();
    ImGui::TextUnformatted(" | ");
    ImGui::SameLine();

    ImGui::TextUnformatted(statekiro::get_current_prosthetic().c_str());

    ImGui::End();
}