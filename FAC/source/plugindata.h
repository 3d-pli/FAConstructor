#ifndef PLUGINDATA_H
#define PLUGINDATA_H

#include <QQuaternion>

namespace FAC {
    class Plugindata
    {
    public:

        static void setRotX(const float &value);
        static float getRotX();

        static float getRotY();
        static void setRotY(const float &value);

        static float getRotZ();
        static void setRotZ(const float &value);

        static void setRotation(const QQuaternion &value);
        static QQuaternion getRotation();

    private:
        static QQuaternion rotation;

        Plugindata();
    };
}

#endif // PLUGINDATA_H
