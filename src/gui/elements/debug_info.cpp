#include <gui/elements/debug_info.hpp>
#include <gui/context.hpp>
#include <config.hpp>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <thread>
#include <hooks.hpp>

namespace SSTux::GUI
{
    void RenderDebugInfo()
    {
        ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // Core Info
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

        // Window Info
        int w, h;
        SDL_GetWindowSize(GetWindow(), &w, &h);
        ImGui::Text("Window Size: %dx%d", w, h);

        // SDL Version
        SDL_version compiled;
        SDL_VERSION(&compiled);
        ImGui::Text("SDL Version: %d.%d.%d", compiled.major, compiled.minor, compiled.patch);

        // OpenGL Info
        const GLubyte *glVersion = glGetString(GL_VERSION);
        const GLubyte *glVendor = glGetString(GL_VENDOR);
        const GLubyte *glRenderer = glGetString(GL_RENDERER);
        const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
        ImGui::Text("OpenGL Version: %s", glVersion ? (const char *)glVersion : "Unknown");
        ImGui::Text("GL Vendor: %s", glVendor ? (const char *)glVendor : "Unknown");
        ImGui::Text("GL Renderer: %s", glRenderer ? (const char *)glRenderer : "Unknown");
        ImGui::Text("GLSL Version: %s", glslVersion ? (const char *)glslVersion : "Unknown");
        ImGui::End();
    }
}
