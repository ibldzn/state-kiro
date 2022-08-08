#pragma once

#include <functional>
#include <string>

extern "C" struct IDXGISwapChain;

namespace statekiro {
    bool initialize();
    void uninitialize();

    unsigned int get_max_health();
    unsigned int get_current_health();

    unsigned int get_max_posture();
    unsigned int get_current_posture();

    std::string get_current_prosthetic();
    std::string get_current_quick_item();

    void register_present_callback(std::function<void(IDXGISwapChain*)> callback);
}