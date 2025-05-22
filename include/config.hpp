#pragma once

#include <string>

namespace SSTux::Config
{
    inline constexpr const char *SSTUX_VERSION = "0.1.7";
    inline const std::string WINDOW_TITLE_PREFIX = std::string(" | SSTux v") + SSTUX_VERSION;
}
