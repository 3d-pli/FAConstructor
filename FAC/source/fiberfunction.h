#ifndef FIBERFUNCTION_H
#define FIBERFUNCTION_H

#include <QString>

namespace FAC {
    typedef float T;

    class FiberFunction
    {
    public:
        FiberFunction();
        FiberFunction(QString functionX, QString functionY, QString functionZ, T min = 0, T max = 0,
                      QString radius = "0.1", T rotX = 0, T rotY = 0, T rotZ = 0, T transformX = 0,
                      T transformY = 0, T transformZ = 0);

        QString functionX();
        QString functionY();
        QString functionZ();

        T min();
        T max();
        QString radius();

        T rotX();
        T rotY();
        T rotZ();

        T transformX();
        T transformY();
        T transformZ();

        bool empty();

    private:
        QString m_functionX, m_functionY, m_functionZ;
        T m_min, m_max;
        QString m_radius;
        T m_rotX, m_rotY, m_rotZ;
        T m_transformX, m_transformY, m_transformZ;

        bool m_empty;
    };
}

#endif // FIBERFUNCTION_H
