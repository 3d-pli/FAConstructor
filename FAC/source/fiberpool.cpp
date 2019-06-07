#include "fiberpool.h"
#include "fiberpoint.h"

FAC::FiberPool* FAC::FiberPool::fiberPool_ = nullptr;

FAC::FiberPool::FiberPool()
{
    this->fibers = std::vector<Fiber>();
    this->toNotify = std::set<QObject*>();
}

void FAC::FiberPool::addFiber(FAC::Fiber& fiber){
    this->fibers.push_back(fiber);
}

void FAC::FiberPool::addFiber(std::vector<FAC::Fiber>& fiber) {
    this->fibers.insert(this->fibers.end(), std::make_move_iterator(fiber.begin()), std::make_move_iterator(fiber.end()));
}

void FAC::FiberPool::deleteFiber(QList<unsigned> index) {
    std::sort(index.begin(), index.end());
    std::reverse(index.begin(), index.end());
    for(auto i : index) {
        deleteFiber(i);
    }
}

void FAC::FiberPool::deleteFiber(unsigned index) {
    if(index > fibers.size()) throw QString("Index is out of bounds!");
    std::vector<std::vector<std::set<unsigned>>::iterator> groupsToDelete;
    #pragma omp parallel for
    for(unsigned i = 0; i < groups.size(); ++i) {
        auto& group = groups.at(i);
        auto found = std::find(group.begin(), group.end(), index);
        if(found != group.end()) {
            group.erase(found);
        }
        // If group is empty. Mark it for removal
        if(group.empty()) groupsToDelete.push_back(groups.begin()+i);
        else {
            // Indice correction if the index of the deleted fiber is in the middle of a given group
            // TODO Revisit this code and check if this can be optimized
            std::vector<unsigned> toInsert;
            for(auto elem = group.rbegin(); elem != group.rend();) {
                if(*elem >= index) {
                    toInsert.push_back((*elem)-1);
                    elem = decltype(elem){ group.erase(std::next(elem).base()) };
                } else {
                    ++elem;
                }
            }
            group.insert(toInsert.begin(), toInsert.end());
        }
    }
    // Remove marked groups
    for(auto group = groupsToDelete.rbegin(); group != groupsToDelete.rend(); ++group) {
        #pragma omp critical
        this->groups.erase(*group);
    }
    #pragma omp critical
    this->fibers.erase(this->fibers.begin()+index);
}

void FAC::FiberPool::createGroup(QList<unsigned>& elements) {
    std::list<unsigned> el = elements.toStdList();
    createGroup(el);
}

void FAC::FiberPool::createGroup(std::list<unsigned>& elements) {
    std::set<unsigned> fibersToAdd;
    fibersToAdd.insert(elements.begin(), elements.end());
    #pragma omp critical
    this->groups.push_back(fibersToAdd);
}

void FAC::FiberPool::createGroup(std::set<unsigned> elements) {
    #pragma omp critical
    this->groups.push_back(elements);
}

void FAC::FiberPool::addFibersToGroup(unsigned list, QList<unsigned>& elements){
    #pragma omp critical
    this->groups.at(list).insert(elements.begin(), elements.end());
}

void FAC::FiberPool::addFibersToGroup(unsigned list, std::list<unsigned>& elements){
    #pragma omp critical
    this->groups.at(list).insert(elements.begin(), elements.end());
}

void FAC::FiberPool::addFibersToGroup(unsigned list, std::set<unsigned>& elements){
    #pragma omp critical
    this->groups.at(list).insert(elements.begin(), elements.end());
}

std::vector<unsigned> FAC::FiberPool::getGroupsContainingFiber(unsigned index) {
    std::vector<unsigned> containingGroups;
    for(unsigned i = 0; i < groups.size(); ++i) {
        if(groups.at(i).find(index) != groups.at(i).end()) {
            containingGroups.push_back(i);
        }
    }
    return containingGroups;
}

void FAC::FiberPool::deleteGroup(unsigned list){
    #pragma omp critical
    this->groups.erase(this->groups.begin()+list);
}

void FAC::FiberPool::deleteFibersFromGroup(unsigned list, QList<unsigned>& elements){
    std::set<unsigned> * fiberList = &groupAt(list);
    for(auto fiber = elements.begin(); fiber != elements.end(); ++fiber) {
        auto found = std::find(fiberList->begin(), fiberList->end(), *fiber);
        if(found != fiberList->end()) {
            fiberList->erase(found);
        }
    }
}

void FAC::FiberPool::clearPool() {
    this->fibers.clear();
    this->groups.clear();
}

void FAC::FiberPool::addObserver(QObject * observer) {
    this->toNotify.insert(observer);
}

void FAC::FiberPool::removeObserver(QObject * observer) {
    auto position = this->toNotify.find(observer);
    if(position != this->toNotify.end()) {
        this->toNotify.erase(position);
    }
}

void FAC::FiberPool::notifyObservers(QEvent::Type type) {
    for(auto it = this->toNotify.begin(); it != this->toNotify.end(); ++it) {
        QEvent event(type);
        QApplication::sendEvent(*it, &event);
    }
}

FAC::FiberPool& FAC::FiberPool::getInstance()  {
    if(fiberPool_ == nullptr) {
        fiberPool_ = new FAC::FiberPool;
    }
    return *fiberPool_;
}
