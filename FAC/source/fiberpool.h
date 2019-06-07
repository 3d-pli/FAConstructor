#ifndef FIBERPOOL_H
#define FIBERPOOL_H
#include <QApplication>
#include <QEvent>
#include <set>
#include <fiber.h>

class QString;
namespace FAC {
    class FiberPool
    {

    public:
        void addFiber(Fiber &fiber);
        void addFiber(std::vector<Fiber>& fibers);
        void deleteFiber(QList<unsigned> index);
        void deleteFiber(unsigned index);

        void createGroup(std::list<unsigned> &elements);
        void createGroup(QList<unsigned>& elements);
        void createGroup(std::set<unsigned> elements);
        void addFibersToGroup(unsigned list, QList<unsigned>& elements);
        void addFibersToGroup(unsigned list, std::list<unsigned>& elements);
        void addFibersToGroup(unsigned list, std::set<unsigned>& elements);
        std::vector<unsigned> getGroupsContainingFiber(unsigned index);
        void deleteGroup(unsigned list);
        void deleteFibersFromGroup(unsigned list, QList<unsigned>& elements);

        void clearPool();

        inline std::vector<Fiber>& getFibers() {
            return fibers;
        }

        inline Fiber& fiberAt(unsigned index) {
            return fibers.at(index);
        }

        inline void replaceFiber(Fiber& fiber, unsigned index) {
            fibers.at(index) = fiber;
        }

        inline std::set<unsigned>& groupAt(unsigned index) {
            return groups.at(index);
        }

        inline std::vector<std::set<unsigned>>& getGroups() {
            return groups;
        }

        // Main notification element. Maybe this should be converted to it's own class.
        void addObserver(QObject * observer);
        void removeObserver(QObject * observer);
        void notifyObservers(QEvent::Type type);
        // Singleton. All classes work with the same instance.
        static FiberPool& getInstance();

    private:
        // Singleton. Therefore the constructor is not accessible from the outside.
        FiberPool();
        static FiberPool * fiberPool_;

        std::vector<Fiber> fibers;
        std::vector<std::set<unsigned>> groups;
        std::set<QObject*> toNotify;
    };
}

#endif // FIBERPOOL_H
