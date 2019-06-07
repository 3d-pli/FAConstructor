#ifndef RENDERDATA_H
#define RENDERDATA_H

#include <vector>
#include "vertex.h"
#include "fiberpool.h"
#include "helpers.h"

namespace FACGUI {
    class RenderData : QObject {
    public:
        static RenderData* getInstance();
        void prepareData();
        void setColor(unsigned);
        std::vector<Vertex> data();
        unsigned color();

        bool event(QEvent *event);
    private:
        RenderData();

        static RenderData* instance;
        FAC::FiberPool* pool;
        std::vector<Vertex> vdata;
        unsigned renderColor;
    };
}

#endif
