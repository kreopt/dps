#include "pal.h"


std::string dps::PAL::path_get_data_dir()
{
    string path_prefix;
#ifdef UNIX
    const char* xdg_data = getenv("XDG_DATA_HOME");
    string home = string(getenv("HOME"));

    path_prefix = xdg_data ? string(xdg_data) : home+"/.local/share";
#elif WINDOWS
    // TODO:
#endif
    return path_prefix;
}

std::string dps::PAL::path_get_config_dir()
{
    string path_prefix;
#ifdef UNIX
    const char* xdg_data = getenv("XDG_CONFIG_HOME");
    string home = string(getenv("HOME"));

    path_prefix = xdg_data ? string(xdg_data) : home+"/.config";
#elif WINDOWS
    // TODO:
#endif
    return path_prefix;
}
