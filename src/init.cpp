#include <cstdio>
#include <thread>
#include <chrono>
#include <hooks.hpp>
#include <gui.hpp>

__attribute__((constructor)) static void Init()
{
    fprintf(stderr, "[SSTux] Hello from SSTux!\n");
    InstallSDLHooks();
    std::thread(InitGUI).detach();
}
