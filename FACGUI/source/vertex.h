#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include "fiberpoint.h"
namespace FACGUI {
    class Vertex
    {
    public:
        Vertex(FAC::FiberPoint point, const int color, const int fiber);
        Vertex(const QVector3D& position, const int color, const int fiber, const float radius);

        const QVector3D& position() const;
        void setPosition(const QVector3D& position);
        int color() const;
        void setColor(const int color);
        int fiber() const;
        void setFiber(const int fiber);
        float radius() const;
        void setRadius(const float radius);

        static const int PositionSize = 3;
        static const int InfoSize = 3;
        static int positionOffset();
        static int infoOffset();
        static int stride();

    private:
        QVector3D m_position;
        QVector3D m_info;
    };
}

#endif // VERTEX_H
