#include <hooks.hpp>
#include <config.hpp>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <cstdlib>
#include <SDL2/SDL.h>

namespace SSTux
{
    namespace
    {
        void Log(const std::string &message)
        {
            std::cerr << "[SSTux] " << message << std::endl;
        }

        std::string GetProcessName()
        {
            char path[PATH_MAX] = {0};
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
            if (len != -1)
            {
                path[len] = '\0';
                return std::string(path);
            }
            return "Unknown";
        }

    }

    __attribute__((constructor)) static void Initialize()
    {
        Log("Initializing SSTux mod for supertux2, loading no mods");

        std::string processName = GetProcessName();
        Log("Injected into process: " + processName);

        if (processName.find("supertux") == std::string::npos)
        {
            Log("Error: SSTux has to be loaded into supertux, assure the binary has \"supertux\" in its name. Aborting.");
            std::exit(EXIT_FAILURE);
        }

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
