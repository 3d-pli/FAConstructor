#include "fiberfunction.h"

FAC::FiberFunction::FiberFunction()
{
    m_empty = true;
}

FAC::FiberFunction::FiberFunction(QString functionX, QString functionY, QString functionZ, T min, T max,
                            QString radius, T rotX, T rotY, T rotZ, T transformX,
                            T transformY, T transformZ)
                            : m_functionX(functionX), m_functionY(functionY), m_functionZ(functionZ), m_min(min), m_max(max),
                              m_radius(radius), m_rotX(rotX), m_rotY(rotY), m_rotZ(rotZ), m_transformX(transformX),
                              m_transformY(transformY), m_transformZ(transformZ), m_empty(false) {}

QString FAC::FiberFunction::functionX() {
    return m_functionX;
}

QString FAC::FiberFunction::functionY() {
    return m_functionY;
}

QString FAC::FiberFunction::functionZ() {
    return m_functionZ;
}

FAC::T FAC::FiberFunction::min() {
    return m_min;
}

FAC::T FAC::FiberFunction::max() {
    return m_max;
}

QString FAC::FiberFunction::radius() {
    return m_radius;
}

FAC::T FAC::FiberFunction::rotX() {
    return m_rotX;
}

FAC::T FAC::FiberFunction::rotY() {
    return m_rotY;
}

FAC::T FAC::FiberFunction::rotZ() {
    return m_rotZ;
}

FAC::T FAC::FiberFunction::transformX() {
    return m_transformX;
}

FAC::T FAC::FiberFunction::transformY() {
    return m_transformY;
}

FAC::T FAC::FiberFunction::transformZ() {
    return m_transformZ;
}

bool FAC::FiberFunction::empty() {
    return m_empty;
}
