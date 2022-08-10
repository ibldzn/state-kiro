#pragma once

#include <singleton.hpp>
#include <vector>

struct View;
struct IDXGISwapChain;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

struct Render : Singleton<Render> {
    static void on_present(IDXGISwapChain* swapchain);

private:
    friend struct Singleton<Render>;
    Render();
    ~Render();

    static bool initialize(IDXGISwapChain* swapchain);

    template <typename T>
    void register_view() { m_views.emplace_back(new T {}); }

private:
    std::vector<View*> m_views = {};
    ID3D11DeviceContext* m_device_context = nullptr;
    ID3D11RenderTargetView* m_render_target_view = nullptr;
};