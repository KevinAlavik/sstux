#include <gui/render.hpp>
#include <gui/context.hpp>
#include <gui/elements/debug_info.hpp>

#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

namespace SSTux::GUI
{
    void BeginRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void EndRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
