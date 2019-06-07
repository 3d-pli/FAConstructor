#include "plugindata.h"

QQuaternion FAC::Plugindata::rotation;

void FAC::Plugindata::setRotX(const float &value)
{
    rotation.setX(value);
}

float FAC::Plugindata::getRotX()
{
    return rotation.x();
}

float FAC::Plugindata::getRotY()
{
    return rotation.y();
}

void FAC::Plugindata::setRotY(const float &value)
{
    rotation.setY(value);
}

float FAC::Plugindata::getRotZ()
{
    return rotation.z();
}

void FAC::Plugindata::setRotZ(const float &value)
{
    rotation.setZ(value);
}

void FAC::Plugindata::setRotation(const QQuaternion &value)
{
    rotation = value;
}

QQuaternion FAC::Plugindata::getRotation()
{
    return rotation;
}
