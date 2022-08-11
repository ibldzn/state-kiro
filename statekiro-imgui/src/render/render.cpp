#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <render/input.hpp>
#include <render/render.hpp>
#include <render/views/views.hpp>

LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Render::Render()
{
    register_view<View>();
    register_view<ViewConfig>();
    register_view<ViewHealthBar>();
    register_view<ViewPostureBar>();
    register_view<ViewQItemProsthetic>();
}

Render::~Render()
{
    for (auto view : m_views) {
        delete view;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Render::on_present(IDXGISwapChain* swapchain)
{
    static const bool initialized = [&] { return initialize(swapchain); }();
    if (!initialized) {
        return;
    }

    const auto& render = Render::instance();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto view : render.m_views) {
        view->draw();
    }

    ImGui::Render();
    render.m_device_context->OMSetRenderTargets(1, &render.m_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool Render::initialize(IDXGISwapChain* swapchain)
{
    ID3D11Device* device = nullptr;
    if (FAILED(swapchain->GetDevice(IID_ID3D11Device, reinterpret_cast<void**>(&device)))) {
        return false;
    }

    auto& render = Render::instance();

    device->GetImmediateContext(&render.m_device_context);

    ID3D11Texture2D* back_buffer = nullptr;
    if (FAILED(swapchain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&back_buffer)))) {
        return false;
    }

    if (FAILED(device->CreateRenderTargetView(back_buffer, nullptr, &render.m_render_target_view))) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC scd = {};
    if (FAILED(swapchain->GetDesc(&scd))) {
        return false;
    }

    if (!Input::instance().initialize(scd.OutputWindow)) {
        return false;
    }

    Input::instance().register_wndproc_callback(
        [](void* hwnd, std::uint32_t msg, std::uintptr_t wparam, std::intptr_t lparam) {
            ImGui_ImplWin32_WndProcHandler(reinterpret_cast<HWND>(hwnd), msg, wparam, lparam);
        }
    );

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(scd.OutputWindow);
    ImGui_ImplDX11_Init(device, render.m_device_context);

    for (auto view : render.m_views) {
        view->on_init();
    }

    return true;
}