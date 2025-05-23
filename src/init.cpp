#include <hooks.hpp>
#include <config.hpp>
#include <dlfcn.h>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <gui/elements.hpp>
#include <fstream>
#include <sstream>

namespace SSTux
{
    void Log(const std::string &message)
    {
        std::cerr << "[SSTux::Core] " << message << std::endl;
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

    uintptr_t GetBaseAddress()
    {
        uintptr_t baseAddress = 0;

        char exePath[PATH_MAX] = {0};
        ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
        if (len == -1)
        {
            Log("Failed to read /proc/self/exe");
            return 0;
        }
        exePath[len] = '\0';
        std::string exeRealPath(exePath);

        std::ifstream maps("/proc/self/maps");
        if (!maps)
        {
            Log("Failed to open /proc/self/maps");
            return 0;
        }

        std::string line;
        while (std::getline(maps, line))
        {
            std::istringstream iss(line);
            std::string addressRange, perms, offset, dev, inode, pathname;
            iss >> addressRange >> perms >> offset >> dev >> inode;
            std::getline(iss, pathname);

            size_t start = pathname.find_first_not_of(' ');
            if (start != std::string::npos)
                pathname = pathname.substr(start);
            else
                pathname.clear();

            if (!pathname.empty() && pathname == exeRealPath && perms.find('x') != std::string::npos)
            {
                size_t dash = addressRange.find('-');
                if (dash != std::string::npos)
                {
                    std::string startAddrStr = addressRange.substr(0, dash);
                    baseAddress = std::stoull(startAddrStr, nullptr, 16);
                    break;
                }
            }
        }

        if (baseAddress == 0)
            Log("Could not find base address of main executable");

        return baseAddress;
    }

    void *GetSymbolAddress(const char *symbol)
    {
        void *handle = dlopen(nullptr, RTLD_NOW);
        if (!handle)
        {
            Log(std::string("dlopen failed: ") + dlerror());
            return nullptr;
        }

        dlerror();
        void *addr = dlsym(handle, symbol);
        char *error = dlerror();
        if (error != nullptr)
        {
            Log(std::string("dlsym error: ") + error);
            dlclose(handle);
            return nullptr;
        }
        dlclose(handle);
        return addr;
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

        uintptr_t baseAddr = GetBaseAddress();
        std::ostringstream oss;
        oss << "SuperTux2 base address: 0x" << std::hex << baseAddr;
        Log(oss.str());

        void *launchGameAddr = GetSymbolAddress("_ZN4Main11launch_gameERK20CommandLineArguments");
        if (launchGameAddr)
        {
            Log("Main::launch_game() symbol address: " + HEXPTR((uintptr_t)launchGameAddr));
        }
        else
        {
            Log("Main::launch_game() symbol not found.");
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
        int major = SSTux::Hooks::GetSuperTuxMajor();
        int minor = SSTux::Hooks::GetSuperTuxMinor();
        int patch = SSTux::Hooks::GetSuperTuxPatch();

        Log("SuperTux v" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch) +
            " with SSTux v" + std::string(SSTux::Config::SSTUX_VERSION) + " exited");
    }
}
