#include <gui/gui.hpp>
#include <gui/context.hpp>
#include <gui/render.hpp>
#include <gui/elements.hpp>

namespace SSTux::GUI
{
    void DrawOverlay()
    {
        if (!InitOverlay())
            return;

        BeginRender();
        RenderDebugInfo();
        EndRender();
    }

    bool Ready()
    {
        return IsOverlayInitialized();
    }
}
