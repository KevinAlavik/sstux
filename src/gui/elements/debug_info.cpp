#include <gui/elements.hpp>
#include <gui/context.hpp>
#include <config.hpp>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <thread>
#include <hooks.hpp>
#include <cstring>

namespace SSTux::GUI
{
    void RenderDebugInfo()
    {
        ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        /* Core Info */ {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("SSTux Version: %s", SSTux::Config::SSTUX_VERSION);
            if (SSTux::Hooks::HasValidSuperTuxVersion())
            {
                ImGui::Text("SuperTux Version: %d.%d.%d",
                            SSTux::Hooks::GetSuperTuxMajor(),
                            SSTux::Hooks::GetSuperTuxMinor(),
                            SSTux::Hooks::GetSuperTuxPatch());
            }
            else
            {
                ImGui::Text("SuperTux Version: Unknown");
            }
        }

        ImGui::Separator();

        /* Rendering info */ {
            ImGui::Text("Window Size: %dx%d", overlayInfo.windowWidth, overlayInfo.windowHeight);
            ImGui::Text("SDL Version: %d.%d.%d", overlayInfo.sdlVersion.major, overlayInfo.sdlVersion.minor, overlayInfo.sdlVersion.patch);
            ImGui::Text("OpenGL Version: %s", overlayInfo.glVersion ? overlayInfo.glVersion : "Unknown");
            ImGui::Text("GL Vendor: %s", overlayInfo.glVendor ? overlayInfo.glVendor : "Unknown");
            ImGui::Text("GL Renderer: %s", overlayInfo.glRenderer ? overlayInfo.glRenderer : "Unknown");
            ImGui::Text("GLSL Version: %s", overlayInfo.glslVersion ? overlayInfo.glslVersion : "Unknown");
        }

        ImGui::End();
    }
}
