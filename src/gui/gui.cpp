#include "gui.h"

#include <thread> 
#include <chrono>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gui 
{
    static HWND window = nullptr;
    static WNDCLASSEXW wc = {};
    static ID3D11Device* device = nullptr;
    static ID3D11DeviceContext* context = nullptr;
    static IDXGISwapChain* swapChain = nullptr;
    static ID3D11RenderTargetView* renderTarget = nullptr;

    void CreateRenderTarget() 
    {
        ID3D11Texture2D* backBuffer = nullptr;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer) 
        {
            device->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);
            backBuffer->Release();
        }
    }

    void CleanupRenderTarget() 
    {
        if (renderTarget) 
        {
            renderTarget->Release();
            renderTarget = nullptr;
        }
    }

    LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) 
        {
            case WM_SIZE:
                if (device && wParam != SIZE_MINIMIZED) 
                {
                    CleanupRenderTarget();
                    swapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget();
                }
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    bool Setup() noexcept 
    {
        wc = {sizeof(WNDCLASSEXW),
              CS_CLASSDC,
              WndProc,
              0L,
              0L,
              GetModuleHandle(nullptr),
              nullptr,
              nullptr,
              nullptr,
              nullptr,
              L"Overlay",
              nullptr};
        RegisterClassExW(&wc);

        window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST, wc.lpszClassName, L"Overlay", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

        SetLayeredWindowAttributes(window, RGB(0, 0, 0), 0, LWA_COLORKEY);

        ShowWindow(window, SW_SHOW);

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = window;
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, 2, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context)))
            return false;

        CreateRenderTarget();

        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(device, context);

        return true;
    }

    void Shutdown() noexcept {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupRenderTarget();
        if (swapChain) {
            swapChain->Release();
            swapChain = nullptr;
        }
        if (context) {
            context->Release();
            context = nullptr;
        }
        if (device) {
            device->Release();
            device = nullptr;
        }

        DestroyWindow(window);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }

    void BeginRender() noexcept {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                isRunning = false;
        }

        if (!isRunning)
            return;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EndRender() noexcept {
        if (!isRunning)
            return;

        ImGui::EndFrame();
        ImGui::Render();

        const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        context->OMSetRenderTargets(1, &renderTarget, nullptr);
        context->ClearRenderTargetView(renderTarget, clearColor);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(limit_fps ? 1 : 0, 0);
    }
}  // namespace gui