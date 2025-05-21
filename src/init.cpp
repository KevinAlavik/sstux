#include <cstdio>
#include <hooks.hpp>

__attribute__((constructor)) static void Init()
{
    fprintf(stderr, "[SuperSuperTux] Hello from SSTux!\n");
    InstallSDLHooks();
}
