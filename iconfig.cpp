#include "iconfig.h"
#include "iniconfig.h"
IConfig* dps::settings(){
    static IConfig* _settings = new IniConfig();
    return _settings;
}
