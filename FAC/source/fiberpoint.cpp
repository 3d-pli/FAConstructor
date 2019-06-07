#include "fiberpoint.h"

FAC::FiberPoint::FiberPoint(float x, float y, float z, float r) : m_x(x), m_y(y), m_z(z), m_r(r) {

}

FAC::FiberPoint::FiberPoint(QVector3D pos, float r) : m_x(pos.x()), m_y(pos.y()), m_z(pos.z()), m_r(r) {

}

FAC::FiberPoint::FiberPoint(QVector4D pos) : m_x(pos.x()), m_y(pos.y()), m_z(pos.z()), m_r(pos.w()) {

}

FAC::FiberPoint::FiberPoint(const FAC::FiberPoint& point) : m_x(point.x()), m_y(point.y()), m_z(point.z()), m_r(point.r()) {

}

FAC::FiberPoint::FiberPoint() : m_x(0), m_y(0), m_z(0), m_r(0) {}
