#ifndef TRANSFORMATION3D_H
#define TRANSFORMATION3D_H

#include <QVector>
#include <QMatrix4x4>
#include <QQuaternion>

namespace FACGUI {
    class Transformation3D
    {
    public:
        Transformation3D();

        void translateBy(const QVector3D& dt);
        void translateBy(float dx, float dy, float dz);

        void scaleBy(const QVector3D& ds);
        void scaleBy(float dx, float dy, float dz);
        void scaleBy(float factor);

        void rotateBy(const QQuaternion& dr);
        void rotateBy(float angle, const QVector3D& axis);
        void rotateBy(float angle, float ax, float ay, float az);

        void growBy(const QVector3D &ds);
        void growBy(float dx, float dy, float dz);
        void growBy(float factor);

        void setTranslation(const QVector3D &t);
        void setTranslation(float x, float y, float z);
        void setScale(const QVector3D &s);
        void setScale(float x, float y, float z);
        void setScale(float k);
        void setRotation(const QQuaternion &r);
        void setRotation(float angle, const QVector3D &axis);
        void setRotation(float angle, float ax, float ay, float az);

        void setMiddlePoint(const QVector3D &m);
        void setMiddlePoint(float x, float y, float z);

        void reset();

        const QVector3D& translation() const;
        const QVector3D& scale() const;
        const QQuaternion& rotation() const;
        const QMatrix4x4& toMatrix();

    private:
        QVector3D m_middlePoint;
        QVector3D m_translation;
        QVector3D m_scale;
        QQuaternion m_rotation;
        QMatrix4x4 m_world;
        bool m_dirty;
    };

    inline Transformation3D::Transformation3D() : m_scale(1.0f, 1.0f, 1.0f), m_dirty(true) {}

    inline void Transformation3D::translateBy(float dx, float dy,float dz) { translateBy(QVector3D(dx, dy, dz)); }
    inline void Transformation3D::scaleBy(float dx, float dy,float dz) { scaleBy(QVector3D(dx, dy, dz)); }
    inline void Transformation3D::scaleBy(float factor) { scaleBy(QVector3D(factor, factor, factor)); }
    inline void Transformation3D::rotateBy(float angle, const QVector3D &axis) { rotateBy(QQuaternion::fromAxisAndAngle(axis, angle)); }
    inline void Transformation3D::rotateBy(float angle, float ax, float ay,float az) { rotateBy(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }
    inline void Transformation3D::growBy(float dx, float dy, float dz) { growBy(QVector3D(dx, dy, dz)); }
    inline void Transformation3D::growBy(float factor) { growBy(QVector3D(factor, factor, factor)); }

    inline void Transformation3D::setTranslation(float x, float y, float z) { setTranslation(QVector3D(x, y, z)); }
    inline void Transformation3D::setScale(float x, float y, float z) { setScale(QVector3D(x, y, z)); }
    inline void Transformation3D::setScale(float k) { setScale(QVector3D(k, k, k)); }
    inline void Transformation3D::setRotation(float angle, const QVector3D &axis) { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
    inline void Transformation3D::setRotation(float angle, float ax, float ay, float az) { setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

    inline void Transformation3D::setMiddlePoint(float x, float y, float z) { setMiddlePoint(QVector3D(x, y, z)); }

    inline const QVector3D& Transformation3D::translation() const { return m_translation; }
    inline const QVector3D& Transformation3D::scale() const { return m_scale; }
    inline const QQuaternion& Transformation3D::rotation() const { return m_rotation; }
}

#endif // TRANSFORMATION3D_H
