#include "gui.h"

namespace menu 
{

    int window_width = 400;
    int window_height = 300;

	void render() noexcept 
	{
        gui::BeginRender();

#ifdef _DEBUG
        ImGui::SetNextWindowSize({(float)window_width, (float)window_height}, ImGuiCond_Always);
#else
        ImGui::SetNextWindowSize({400, 300}, ImGuiCond_FirstUseEver);
#endif
        if (ImGui::Begin("Minimal Overlay", &gui::isRunning, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            ImGui::Checkbox("Limit Overlay FPS", &gui::limit_fps);

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(
                    "Unchecked: Uncapped FPS, High Performance \n"
                    "Checked:   Automatically lowers overlay fps, Saves Power");
                ImGui::EndTooltip();
            }

            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            #ifdef _DEBUG
            ImGui::Spacing();
            ImGui::SliderInt("Width", &window_width, 200, 800);
            ImGui::SliderInt("Height", &window_height, 150, 600);
            #endif

        }
        ImGui::End();

        gui::EndRender();
     }

}