#pragma once

#include <dlfcn.h>
#include <stdexcept>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <SDL2/SDL.h>

namespace SSTux::Hooks
{
    inline void Log(const std::string &message)
    {
        std::cerr << "[SSTux::Hooks] " << message << std::endl;
    }

    class HookManager
    {
    public:
        template <typename FuncType>
        void RegisterHook(const std::string &symbol, FuncType &real_func)
        {
            void *handle = dlsym(RTLD_NEXT, symbol.c_str());
            if (!handle)
            {
                throw std::runtime_error("Failed to resolve symbol: " + symbol);
            }
            real_func = reinterpret_cast<FuncType>(handle);
            hooked_functions_[symbol] = handle;
            Log("Hooked symbol: " + symbol);
        }

        template <typename FuncType>
        FuncType GetRealFunction(const std::string &symbol) const
        {
            auto it = hooked_functions_.find(symbol);
            if (it == hooked_functions_.end())
            {
                throw std::runtime_error("Symbol not hooked: " + symbol);
            }
            return reinterpret_cast<FuncType>(it->second);
        }

        bool IsHooked(const std::string &symbol) const
        {
            return hooked_functions_.find(symbol) != hooked_functions_.end();
        }

    private:
        std::unordered_map<std::string, void *> hooked_functions_;
    };

    inline HookManager &GetHookManager()
    {
        static HookManager instance;
        return instance;
    }

    // SDL
    void InstallSDLHooks();
    SDL_Window *GetStoredWindow();
    SDL_GLContext GetStoredGLContext();
    int GetSuperTuxMajor();
    int GetSuperTuxMinor();
    int GetSuperTuxPatch();
    bool HasValidSuperTuxVersion();
} // namespace SSTux::Hooks