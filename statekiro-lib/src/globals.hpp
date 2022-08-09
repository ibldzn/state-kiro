#pragma once

#include <statekiro.hpp>
#include <string>

namespace globals {
    inline unsigned int max_health = 0;
    inline unsigned int current_health = 0;

    inline unsigned int max_posture = 0;
    inline unsigned int current_posture = 0;

    inline void* local_player = nullptr;

    inline std::string current_quick_item = {};
    inline std::string current_prosthetic = {};

    inline std::vector<statekiro::present_callback_t> presents_callbacks = {};
}
