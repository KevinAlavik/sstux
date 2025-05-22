#include <hooks.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <config.hpp>

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
        Log("Initializing SSTux mod for supertux2, loading no mods");

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
        Log("SuperTux v?.?.? with SSTux v" + std::string(SSTux::Config::SSTUX_VERSION) + " exited");
    }
}