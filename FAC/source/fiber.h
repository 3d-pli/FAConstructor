#ifndef FIBER_H
#define FIBER_H

#include <vector>
#include <QVector>
#include <QMatrix4x4>
#include <fiberpoint.h>
#include <fiberfunction.h>
#include <helpers.h>
#include <exprtk.hpp>

namespace FAC {
    /* The fiber class represents the main component to store fiber data. Most manipulations are done here
     * by using the given methods. */
    class Fiber
    {
    public:
        // Create fiber with given size to allocate memory
        Fiber(const unsigned long long size = 0);
        // Copy constructor
        Fiber(const Fiber& fiber);
        // Create fiber with given FiberPoints
        Fiber(std::vector<FiberPoint>& parts);
        Fiber(QVector<FiberPoint>& parts);

        // Retrieve FiberPoints
        std::vector<FiberPoint> getParts() const;
        // Check if Fiber is shown during rendering
        bool isActive() const;
        // Add fiber point to bundle
        void addPoint(FiberPoint point);
        // Set if fiber bundle is shown
        void setActive(bool active);
        // Change fiber point in bundle
        void setFiberPoint(const unsigned at, const FiberPoint point);

        void translate(QVector3D& dimensions);
        void translate(float x, float y, float z);
        void rotate(float x_angle, float y_angle, float z_angle);

        // Remove every "by"-th FiberPoint from dataPoints
        void thin(unsigned by);

        void setCreatedByFunction(FiberFunction function);
        FiberFunction function() const;

        static Fiber createByFunction(FiberFunction function, float stepsize);
    private:
        std::vector<FiberPoint> dataPoints;
        // Fiber is considered active in the program and is therefore shown
        bool active;

        // If parametric functions were used to describe a fiber bundle the information is stored here while the model is not saved yet
        FiberFunction m_function;
    };
}

#endif // FIBER_H
