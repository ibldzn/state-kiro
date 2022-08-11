#pragma once

#include <cstdint>
#include <render/input.hpp>

struct View {
    virtual void on_init();
    virtual void draw();
    virtual ~View() = default;

protected:
    template <typename Func>
    requires requires(Func func) { func(); }
    void RegisterHotkey(unsigned int hotkey, Func&& func)
    {
        Input::instance().register_wndproc_callback(
            [hotkey, func = std::move(func)](void*, std::uint32_t msg, std::uintptr_t wparam, std::intptr_t) {
                if (msg == 0x0101 /*WM_KEYUP*/ && wparam == hotkey) {
                    func();
                }
            }
        );
    }

    inline static bool m_show_config_window = false;
    inline static bool m_show_health_bar = false;
    inline static bool m_show_posture_bar = false;
    inline static bool m_show_quick_item_prosthetic = false;
};

struct ViewConfig : public View {
    void on_init() override;
    void draw() override;
};

struct ViewHealthBar : public View {
    void on_init() override;
    void draw() override;
};

struct ViewPostureBar : public View {
    void on_init() override;
    void draw() override;
};

struct ViewQItemProsthetic : public View {
    void on_init() override;
    void draw() override;
};