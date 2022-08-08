#include "globals.hpp"
#include "hooks/hooks.hpp"
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