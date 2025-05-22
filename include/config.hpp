#pragma once

#include <string>

namespace SSTux::Config
{
    inline constexpr const char *SSTUX_VERSION = "0.1.8";
    inline const std::string WINDOW_TITLE_PREFIX = std::string(" | SSTux v") + SSTUX_VERSION;
}

#define HEXPTR(x) ([](auto ptr) { \
    std::ostringstream oss; \
    oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr); \
    return oss.str(); }(x))
