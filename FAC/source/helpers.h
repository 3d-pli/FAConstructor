#ifndef HELPERS_H
#define HELPERS_H

#include <QEvent>
#include <QMatrix3x3>
#include <QVector4D>
#include <QVector3D>
#ifndef WITHOUT_BOOST
    #include <boost/math/tr1.hpp>
    #include <boost/math/interpolators/cubic_b_spline.hpp>
#endif
#include "fiberpoint.h"
#include "fiber.h"
#ifdef WIN32
	#define _USE_MATH_DEFINES
	#include <math.h>
#endif
namespace FAC {
    /**********************************
     *          SIMPLE MATH
     **********************************/

    template<class T>
    T pow2C(const T& value) {
        return value * value;
    }

    /**********************************
     *           STRUCTURES
     **********************************/

    struct float3 {
        float x;
        float y;
        float z;

        float3(float x, float y, float z) : x(x), y(y), z(z) {}
        float3() : x(0), y(0), z(0) {}
    };

    /***********************************
     *             EVENTS
     **********************************/
    namespace UserEvent {
        const QEvent::Type DocumentEdited = QEvent::Type(1001);
        const QEvent::Type FiberHidden = QEvent::Type(1002);
        const QEvent::Type CameraMoved = QEvent::Type(1003);
        const QEvent::Type PluginCalled = QEvent::Type(1004);
        const QEvent::Type RenderDataUpdated = QEvent::Type(1005);
    }

    /***********************************
     *          INTERPOLATION
     ***********************************/
    namespace Interpolation {
    #ifndef WITHOUT_BOOST
        template<typename T>
        void CubicInterpolate(std::vector<T>& data, unsigned long long steps){
            using namespace boost::math;
            std::vector<float> x(data.size());
            std::vector<float> y(data.size());
            std::vector<float> z(data.size());
            std::vector<float> r(data.size());

            #pragma omp parallel for
            for(unsigned i = 0; i < data.size(); ++i) {
                x.at(i) = data.at(i).x();
                y.at(i) = data.at(i).y();
                z.at(i) = data.at(i).z();
                r.at(i) = data.at(i).w();
            }
            data.clear();

            cubic_b_spline<float> spline_x(x.begin(), x.end(), 0, 1);
            cubic_b_spline<float> spline_y(y.begin(), y.end(), 0, 1);
            cubic_b_spline<float> spline_z(z.begin(), z.end(), 0, 1);
            cubic_b_spline<float> spline_r(r.begin(), r.end(), 0, 1);

            data.resize(steps*(r.size()-1));
            #pragma omp parallel for
            for(unsigned i = 0; i < r.size()-1; ++i) {
                for(unsigned j = 0; j < steps; ++j) {
                    float position = i+float(j)/steps;
                    data.at(i*steps+j) = T(spline_x(position), spline_y(position), spline_z(position), spline_r(position));
                }
            }
        }

        template void CubicInterpolate<QVector4D>(std::vector<QVector4D>&, unsigned long long steps);
        template void CubicInterpolate<FiberPoint>(std::vector<FiberPoint>&, unsigned long long steps);
    #else
        template<typename T>
        T CubicInterpolatePoints(T y0, T y1, T y2, T y3, float mu) {
            float mu2;
            T a0, a1, a2, a3;
            mu2 = mu*mu;
            a0 = y3 - y2 - y0 + y1;
            a1 = y0 - y1 - a0;
            a2 = y2 - y0;
            a3 = y1;
            return(a0*mu*mu2+a1*mu2+a2*mu+a3);
        }

        template QVector3D CubicInterpolatePoints(QVector3D y0, QVector3D y1, QVector3D y2, QVector3D y3, float mu);
        template QVector4D CubicInterpolatePoints(QVector4D y0, QVector4D y1, QVector4D y2, QVector4D y3, float mu);

        template<typename T>
        void CubicInterpolate(std::vector<T>& data, unsigned long long steps){
            std::vector<T> new_data;
            QVector4D y0, y1, y2, y3;
            y1 = data.at(0);
            y2 = data.at(1);
            y0 = y1 - (y2 - y1);
            y3 = data.at(2);

            for(unsigned step = 0; step < steps; ++step) {
                float mu = float(step) / steps;
                new_data.push_back(CubicInterpolatePoints(y0, y1, y2, y3, mu));
            }

            // Calculation of entry 1 to size-2

            for(unsigned i = 0; i < data.size()-3; ++i) {
                y0 = data.at(i);
                y1 = data.at(i+1);
                y2 = data.at(i+2);
                y3 = data.at(i+3);

                for(unsigned step = 0; step < steps; ++step) {
                    float mu = float(step) / steps;
                    new_data.push_back(CubicInterpolatePoints(y0, y1, y2, y3, mu));
                }
            }

            y0 = data.at(data.size()-3);
            y1 = data.at(data.size()-2);
            y2 = data.at(data.size()-1);
            y3 = y2 + (y2 - y1);

            for(unsigned step = 0; step < steps; ++step) {
                float mu = float(step) / steps;
                new_data.push_back(CubicInterpolatePoints(y0, y1, y2, y3, mu));
            }

            data.clear();
            data.swap(new_data);
            new_data.clear();
        }

        template void CubicInterpolate<QVector4D>(std::vector<QVector4D>&, unsigned long long steps);
        template void CubicInterpolate<FiberPoint>(std::vector<FiberPoint>&, unsigned long long steps);
    #endif

        template<typename T>
        void LinearRegression(std::vector<T>& data, float stepsize){
            QVector4D begin;
            QVector4D end;
            QVector4D step;
            unsigned factor;
            auto old_size = data.size();

            std::vector<T> new_data;

            for(unsigned i = 0; i < old_size-1; ++i) {
                begin = data.at(i);
                end = data.at(i+1);

                new_data.push_back(data.at(i));

                step = end - begin;
                if(step.length() < stepsize) continue;
                factor = unsigned(step.toVector3D().lengthSquared() / stepsize);
                step = step/factor;

                for(unsigned j = 1; j < factor; ++j) {
                    new_data.push_back(T(begin + step * j));
                }
            }
            new_data.push_back(data.at(old_size-1));
            data.clear();
            data.insert(data.end(), new_data.begin(), new_data.end());
        }

        template void LinearRegression<QVector4D>(std::vector<QVector4D>&, float);
        template void LinearRegression<FiberPoint>(std::vector<FiberPoint>&, float);
    }

    /***********************************
     *          BUNDLECREATION
     ***********************************/

    namespace BundleCreation {
        std::vector<Fiber> CreateFibersFromFiberBundle(Fiber f, float radius, float distance);
    }
}
#endif // HELPERS_H
