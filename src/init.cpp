#include <hooks.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace SSTux
{

    namespace
    {
        void Log(const std::string &message)
        {
            std::cerr << "[SSTux] " << message << std::endl;
        }
    }

    __attribute__((constructor)) static void Initialize()
    {
        Log("Initializing SSTux cheat for SuperTux2");

        try
        {
            Hooks::InstallSDLHooks();
            Log("Hooks installed successfully");
        }
        catch (const std::exception &e)
        {
            Log("Initialization failed: " + std::string(e.what()));
        }
    }

    __attribute__((destructor)) static void Terminate()
    {
        Log("SuperTux v?.?.? with SSTux v1.0.0 exited");
    }

} // namespace SSTux