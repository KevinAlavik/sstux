#include <gui/elements.hpp>
#include <gui/context.hpp>
#include <config.hpp>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <thread>
#include <hooks.hpp>
#include <cstring>
#include <sstux.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace SSTux::GUI
{
    // Utility function to check if an address is readable by parsing /proc/self/maps
    bool IsAddressMapped(uintptr_t addr)
    {
        std::ifstream maps("/proc/self/maps");
        std::string line;

        while (std::getline(maps, line))
        {
            std::istringstream iss(line);
            std::string address_range;
            std::string perms;

            if (!(iss >> address_range >> perms))
                continue;

            // Skip non-readable regions
            if (perms.find('r') == std::string::npos)
                continue;

            size_t dash = address_range.find('-');
            if (dash == std::string::npos)
                continue;

            uintptr_t start = std::stoull(address_range.substr(0, dash), nullptr, 16);
            uintptr_t end = std::stoull(address_range.substr(dash + 1), nullptr, 16);

            if (addr >= start && addr < end)
                return true;
        }

        return false;
    }

    void RenderDebugInfo()
    {
        ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        /* Core Info */
        {
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

        /* Rendering info */
        {
            ImGui::Text("Window Size: %dx%d", overlayInfo.windowWidth, overlayInfo.windowHeight);
            ImGui::Text("SDL Version: %d.%d.%d", overlayInfo.sdlVersion.major, overlayInfo.sdlVersion.minor, overlayInfo.sdlVersion.patch);
            ImGui::Text("OpenGL Version: %s", overlayInfo.glVersion ? overlayInfo.glVersion : "Unknown");
            ImGui::Text("GL Vendor: %s", overlayInfo.glVendor ? overlayInfo.glVendor : "Unknown");
            ImGui::Text("GL Renderer: %s", overlayInfo.glRenderer ? overlayInfo.glRenderer : "Unknown");
            ImGui::Text("GLSL Version: %s", overlayInfo.glslVersion ? overlayInfo.glslVersion : "Unknown");
        }

        ImGui::Separator();

        /* Game info */
        {
            ImGui::Text("Game Base Address: 0x%llx", SSTux::GetBaseAddress());
        }

        ImGui::End();
    }
}
