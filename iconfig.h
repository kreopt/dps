#ifndef ICONFIG_H
#define ICONFIG_H

#include <string>

using namespace std;
class IConfig
{
public:
    virtual void        set_value(const string& key, const string& val) = 0;
    virtual void        set_value(const string& key, int val) = 0;
    virtual void        set_value(const string& key, bool val) = 0;

    virtual string      get_value(const string& key, const string& default_val = "") const = 0;
    virtual int         get_value_int(const string& key, int default_val = 0) const = 0;
    virtual bool        get_value_bool(const string& key, bool default_val = false) const = 0;

    virtual void        start_group(const string& group_name) = 0;
    virtual void        end_group() = 0;
};
namespace dps {
    IConfig* settings();
}
#endif // ICONFIG_H
