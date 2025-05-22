#include <iostream>

namespace SSTux
{
    namespace
    {
        void Log(const std::string &message)
        {
            std::cerr << "[SSTux] " << message << std::endl;
        }

    }

    void Update(void)
    {
        /* This function gets called right before SDL_GL_SwapWindow */
    }
}