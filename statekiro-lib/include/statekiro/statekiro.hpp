#pragma once

#include <functional>
#include <string>
#include <vector>

extern "C" struct IDXGISwapChain;

namespace statekiro {
    using present_callback_t = std::function<void(IDXGISwapChain*)>;

    bool initialize();
    void shutdown();

    unsigned int get_max_health();
    unsigned int get_current_health();

    unsigned int get_max_posture();
    unsigned int get_current_posture();

    std::string get_current_prosthetic();
    std::string get_current_quick_item();

    void register_present_callback(present_callback_t callback);
    const std::vector<present_callback_t>& get_present_callbacks();
}