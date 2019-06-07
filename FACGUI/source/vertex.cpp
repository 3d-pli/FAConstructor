#include "vertex.h"

FACGUI::Vertex::Vertex(FAC::FiberPoint point, const int color, const int fiber) {
    this->m_position = QVector3D(point.x(), point.y(), point.z());
    this->m_info.setX(color);
    this->m_info.setY(fiber);
    this->m_info.setZ(point.r());
}

FACGUI::Vertex::Vertex(const QVector3D& position, const int color, const int fiber, const float radius) : m_position(position) {
    this->m_info.setX(color);
    this->m_info.setY(fiber);
    this->m_info.setZ(radius);
}

int FACGUI::Vertex::color() const {
    return int(this->m_info.x());
}

const QVector3D& FACGUI::Vertex::position() const {
    return this->m_position;
}

int FACGUI::Vertex::fiber() const {
    return int(this->m_info.y());
}

float FACGUI::Vertex::radius() const {
    return this->m_info.z();
}

inline void FACGUI::Vertex::setPosition(const QVector3D& position) {
    m_position = position;
}

inline void FACGUI::Vertex::setColor(const int color) {
    m_info.setX(color);
}

inline void FACGUI::Vertex::setFiber(const int fiber) {
    m_info.setY(fiber);
}

inline void FACGUI::Vertex::setRadius(const float radius) {
    m_info.setZ(radius);
}

int FACGUI::Vertex::positionOffset() {
    return offsetof(Vertex, m_position);
}

int FACGUI::Vertex::infoOffset() {
    return offsetof(Vertex, m_info);
}

int FACGUI::Vertex::stride() {
    return sizeof(Vertex);
}
