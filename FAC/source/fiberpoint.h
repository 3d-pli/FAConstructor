#ifndef FIBERPOINT_H
#define FIBERPOINT_H

#include <string>
#include <algorithm>
#include <QVector3D>
#include <QVector4D>

namespace FAC {
    class Fiber;

    class FiberPoint
    {
    public:
        FiberPoint(float x, float y, float z, float r);
        FiberPoint(QVector3D pos, float r);
        FiberPoint(QVector4D pos);
        FiberPoint(const FiberPoint& point);
        FiberPoint();

        inline float x() const {
            return m_x;
        }

        inline float y() const {
            return m_y;
        }

        inline float z() const {
            return m_z;
        }

        inline float r() const {
            return m_r;
        }

        inline float w() const {
            return m_r;
        }

        operator std::string() {
            std::string returnValue = std::to_string(m_x) + "\t" + std::to_string(m_y) + "\t" + std::to_string(m_z) + "\t" + std::to_string(m_r) + "\n";
            std::replace(returnValue.begin(), returnValue.end(), ',', '.');
            return returnValue;
        }

        operator QVector3D() {
            return QVector3D(m_x, m_y, m_z);
        }

        inline QVector3D toQVector3D() {
            return QVector3D(m_x, m_y, m_z);
        }

        inline QVector3D toVector3D() {
            return QVector3D(m_x, m_y, m_z);
        }

        operator QVector4D() {
            return QVector4D(m_x, m_y, m_z, m_r);
        }

        inline QVector4D toQVector4D() {
            return QVector4D(m_x, m_y, m_z, m_r);
        }

        inline QVector4D toVector4D() {
            return QVector4D(m_x, m_y, m_z, m_r);
        }

    private:
        float m_x;
        float m_y;
        float m_z;
        float m_r;

        friend class Fiber;
    };
}

#endif // FIBERPOINT_H
