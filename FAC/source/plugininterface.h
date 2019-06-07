#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>

class FACPluginInterface {
public:
    virtual ~FACPluginInterface() {}

    virtual void run() = 0;
};

#define FACPluginInterface_iid "org.inm-1.FAConstructor.FACPluginInterface"
Q_DECLARE_INTERFACE(FACPluginInterface, FACPluginInterface_iid)

#endif // PLUGININTERFACE_H
