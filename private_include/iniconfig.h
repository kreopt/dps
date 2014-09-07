#ifndef INICONFIG_H
#define INICONFIG_H

#include <QSettings>
#include "pal.h"
#include "iconfig.h"

class IniConfig : public IConfig
{
    QSettings *settings;

    void                set_value(const string& key, const QVariant& val);
    QVariant            get_value_qvariant(const string& key, const QVariant& default_val) const;
public:
    IniConfig();
    ~IniConfig();

    virtual void        set_value(const string& key, const string& val) override;
    virtual void        set_value(const string& key, int val) override;
    virtual void        set_value(const string& key, bool val) override;

    virtual string      get_value(const string& key, const string& default_val = "") const override;
    virtual int         get_value_int(const string& key, int default_val = 0) const override;
    virtual bool        get_value_bool(const string& key, bool default_val = false) const override;

    virtual void        start_group(const string& group_name) override;
    virtual void        end_group() override;
};

#endif // INICONFIG_H
