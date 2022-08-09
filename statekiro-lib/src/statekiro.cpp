#include "globals.hpp"
#include "hooks/hooks.hpp"
#include <mutex>
#include <statekiro.hpp>

bool statekiro::initialize()
{
    return Hooks::instance().initialize();
}

void statekiro::uninitialize()
{
    Hooks::instance().shutdown();
}

unsigned int statekiro::get_max_health()
{
    return globals::max_health;
}

unsigned int statekiro::get_current_health()
{
    return globals::current_health;
}

unsigned int statekiro::get_max_posture()
{
    return globals::max_posture;
}

unsigned int statekiro::get_current_posture()
{
    return globals::current_posture;
}

std::string statekiro::get_current_prosthetic()
{
    return globals::current_prosthetic;
}

std::string statekiro::get_current_quick_item()
{
    return globals::current_quick_item;
}

void statekiro::register_present_callback(statekiro::present_callback_t callback)
{
    static std::mutex mtx;
    std::lock_guard lock(mtx);
    globals::presents_callbacks.emplace_back(std::move(callback));
}