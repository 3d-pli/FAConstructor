#include "transformation3d.h"

void FACGUI::Transformation3D::translateBy(const QVector3D &dt) {
    m_dirty = true;
    m_translation += dt;
}

void FACGUI::Transformation3D::scaleBy(const QVector3D &ds) {
    m_dirty = true;
    m_scale *= ds;
}

void FACGUI::Transformation3D::rotateBy(const QQuaternion &dr) {
    m_dirty = true;
    m_rotation = dr * m_rotation;
}

void FACGUI::Transformation3D::growBy(const QVector3D &ds) {
    m_dirty = true;
    m_scale += ds;
}

void FACGUI::Transformation3D::setTranslation(const QVector3D &t)
{
  m_dirty = true;
  m_translation = t;
}

void FACGUI::Transformation3D::setScale(const QVector3D &s)
{
  m_dirty = true;
  m_scale = s;
}

void FACGUI::Transformation3D::setRotation(const QQuaternion &r)
{
  m_dirty = true;
  m_rotation = r;
}

void FACGUI::Transformation3D::setMiddlePoint(const QVector3D &m) {
  m_dirty = true;
  m_middlePoint = m;
}

void FACGUI::Transformation3D::reset() {
    m_translation = QVector3D(0, 0, 0);
    m_middlePoint = QVector3D(0, 0, 0);
    m_scale = QVector3D(1, 1, 1);
    m_rotation = QQuaternion();
}

const QMatrix4x4& FACGUI::Transformation3D::toMatrix() {
    if(m_dirty) {
        m_dirty = false;
        m_world.setToIdentity();
        m_world.translate(m_translation);
        m_world.rotate(m_rotation);
        m_world.scale(m_scale);
        m_world.translate(m_middlePoint);
    }
    return m_world;
}
