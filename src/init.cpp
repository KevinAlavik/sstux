#include <cstdio>
#include <thread>
#include <chrono>
#include <hooks.hpp>

__attribute__((constructor)) static void Init()
{
    fprintf(stderr, "[SSTux] Hello from SSTux!\n");
    InstallSDLHooks();
}
