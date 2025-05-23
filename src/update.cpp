#include <iostream>

namespace SSTux
{
    namespace
    {
        [[maybe_unused]] void Log(const std::string &message)
        {
            std::cerr << "[SSTux::Update] " << message << std::endl;
        }

    }

    void Update(void)
    {
        /* This function gets called right before SDL_GL_SwapWindow, every frame */
    }
}