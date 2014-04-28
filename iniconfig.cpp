#include "iniconfig.h"

IniConfig::IniConfig()
{
    string config_path = dps::PAL::path_get_config_dir()+"/dps.conf";

    this->settings = new QSettings(QString::fromStdString(config_path), QSettings::IniFormat);
}
IniConfig::~IniConfig(){
    delete this->settings;
}

void IniConfig::set_value(const string &key, const string &val)
{
    this->set_value(key, QString::fromStdString(val));
}

void IniConfig::set_value(const string &key, int val)
{
    this->set_value(key, QVariant(val));
}

void IniConfig::set_value(const string &key, bool val)
{
    this->set_value(key, QVariant(val));
}

void IniConfig::set_value(const string &key, const QVariant &val)
{
    this->settings->setValue(QString::fromStdString(key), val);
}

QVariant IniConfig::get_value_qvariant(const string &key, const QVariant& default_value) const
{
    return this->settings->value(QString::fromStdString(key),default_value);
}

string IniConfig::get_value(const string &key, const string& default_val) const
{
    return this->get_value_qvariant(key, QString::fromStdString(default_val)).toString().toStdString();
}

int IniConfig::get_value_int(const string &key, int default_val) const
{
    return this->get_value_qvariant(key, QVariant(default_val)).toInt();
}

bool IniConfig::get_value_bool(const string &key, bool default_val) const
{
    return this->get_value_qvariant(key, default_val).toBool();
}

void IniConfig::start_group(const string &group_name)
{
    this->settings->beginGroup(QString::fromStdString(group_name));
}

void IniConfig::end_group()
{
    this->settings->endGroup();
}
