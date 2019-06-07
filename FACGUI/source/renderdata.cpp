#include "renderdata.h"

FACGUI::RenderData* FACGUI::RenderData::instance = nullptr;

FACGUI::RenderData::RenderData() {
    pool = &FAC::FiberPool::getInstance();
    pool->addObserver(this);
    this->renderColor = 0;
}

FACGUI::RenderData *FACGUI::RenderData::getInstance()
{
    if(instance == nullptr) {
        instance = new RenderData;
    }
    return instance;
}

void FACGUI::RenderData::prepareData()
{
    vdata.clear();
    auto fiberPool = pool->getFibers();

#ifndef WIN32
    #pragma omp declare reduction(merge : std::vector<Vertex> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
    #pragma omp parallel for reduction(merge : vdata)
#endif
    for(unsigned i = 0; i < fiberPool.size(); ++i) {
        auto fiber = fiberPool.at(i);
        if(!fiber.isActive()) continue;
        auto fiberParts = fiber.getParts();
        if(fiberParts.empty()) continue;

        std::vector<Vertex> tempData;

        tempData.push_back(Vertex(fiberParts.at(0), this->renderColor, i));
        for(unsigned j = 1; j < fiberParts.size(); ++j) {
            auto part = fiberParts.at(j);
            tempData.push_back(Vertex(part, this->renderColor, i));
            tempData.push_back(Vertex(part, this->renderColor, i));
        }
        tempData.pop_back();
        vdata.insert(vdata.end(), tempData.begin(), tempData.end());
    }

    pool->notifyObservers(FAC::UserEvent::RenderDataUpdated);
}

void FACGUI::RenderData::setColor(unsigned color) {
    this->renderColor = color;
}

unsigned FACGUI::RenderData::color() {
    return renderColor;
}

std::vector<FACGUI::Vertex> FACGUI::RenderData::data() {
    return vdata;
}

bool FACGUI::RenderData::event(QEvent* event) {
    if(event->type() > 1000 && event->type() != 1005) {
        this->prepareData();
        return true;
    } else return QObject::event(event);
}
