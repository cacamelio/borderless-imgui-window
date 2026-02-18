#include <thread>

#include "gui/gui.h"
#include "../external/imgui-legacy/imgui.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR args, int cmdShow) {
    if (!gui::Setup())
        return EXIT_FAILURE;

    while (gui::isRunning) 
    {
        menu::render();
    }

    gui::Shutdown();
    return EXIT_SUCCESS;
}