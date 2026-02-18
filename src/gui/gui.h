#pragma once

#include "../../external/imgui-legacy/imgui.h"
#include "../../external/imgui-legacy/imgui_impl_dx11.h"
#include "../../external/imgui-legacy/imgui_impl_win32.h"

#include <d3d11.h>

namespace gui {
    inline bool isRunning = true;  // Flag to keep the loop going
    inline bool limit_fps = false;

    bool Setup() noexcept;
    void Shutdown() noexcept;

    void BeginRender() noexcept;
    void EndRender() noexcept;
}  // namespace gui

namespace menu
{
    void render() noexcept;
}  // namespace menu