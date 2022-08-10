#include "globals.hpp"
#include "hooks/hooks.hpp"
#include <mutex>
#include <statekiro.hpp>

static std::vector<statekiro::present_callback_t> presents_callbacks = {};

bool statekiro::initialize()
{
    static const bool once = [] { return Hooks::instance().initialize(); }();
    return once;
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
    presents_callbacks.emplace_back(std::move(callback));
}

const std::vector<statekiro::present_callback_t>& statekiro::get_present_callbacks()
{
    return presents_callbacks;
}