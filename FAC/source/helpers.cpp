#include "helpers.h"
namespace FAC {
    namespace BundleCreation {

        namespace {
            QMatrix3x3 RotateFromAToB(QVector3D a, QVector3D b) {
                QVector3D v = QVector3D::crossProduct(a, b);
                float s = 1e-9f + v.length();
                float c = QVector3D::dotProduct(a, b);
                std::array<float, 9> values = {0, -v.z(), v.y(), v.z(), 0, -v.x(), -v.y(), v.x(), 0};
                QMatrix3x3 vx(values.data());
                QMatrix3x3 ident;
                ident.setToIdentity();
                QMatrix3x3 rotation = ident + vx + vx * vx * (1-c)/FAC::pow2C(s);
                return rotation;
            }

            QVector3D MatrixDotVector(QMatrix3x3 mat, QVector3D vec) {
                QVector3D result;
                result.setX(mat.data()[0] * vec.x() + mat.data()[3] * vec.y() + mat.data()[6] * vec.z());
                result.setY(mat.data()[1] * vec.x() + mat.data()[4] * vec.y() + mat.data()[7] * vec.z());
                result.setZ(mat.data()[2] * vec.x() + mat.data()[5] * vec.y() + mat.data()[8] * vec.z());
                return result;
            }

            std::vector<QVector3D> PopulateCircle(float circleRadius, float distance) {
                std::vector<float> x0((2*circleRadius)/distance);
                std::vector<float> y0((2*circleRadius)/(2*cos(1./6*M_PI)*distance)+1);

                #pragma omp parallel for
                for(unsigned i = 0; i < x0.size(); ++i) {
                    x0.at(i) = -circleRadius + i * distance;
                }
                #pragma omp parallel for
                for(unsigned i = 0; i < y0.size(); ++i) {
                    y0.at(i) = -circleRadius + i * 2*cos(1./6*M_PI)*distance;
                }

                float dx = sin(1./6*M_PI)*distance;
                float dy = cos(1./6*M_PI)*distance;
                float r2 = FAC::pow2C(circleRadius);

                std::vector<QVector3D> values;
                values.reserve(x0.size()*y0.size()*2*0.78); // fraction of circle in square
                for(unsigned i = 0; i < x0.size(); ++i) {
                    for(unsigned j = 0; j < y0.size(); ++j) {
                       if(FAC::pow2C(x0.at(i)) + FAC::pow2C(y0.at(j)) <= r2)
                         values.push_back(QVector3D(x0.at(i), y0.at(j), 0));
                       if(FAC::pow2C(x0.at(i)+dx) + FAC::pow2C(y0.at(j)+dy) <= r2)
                         values.push_back(QVector3D(x0.at(i)+dx, y0.at(j)+dy, 0));

                    }
                }
                return values;
            }


            std::vector<FAC::Fiber> FormFibersAlongBundle(std::vector<QVector3D> shape, FAC::Fiber fiber, float radius, float min_radius) {
                std::vector<FAC::FiberPoint> bundlePath = fiber.getParts();

                //for all points of the fiber bundle
                    //for all points of the populated circle
                std::vector<FAC::Fiber> filledBundle(shape.size());

                QVector3D tangent_old(0, 0, 1), tangent_new;
                float radius_old = min_radius;

                tangent_new = bundlePath.at(1).toQVector3D() - bundlePath.at(0).toQVector3D();
                tangent_new.normalize();
                QMatrix3x3 rotation = RotateFromAToB(tangent_old, tangent_new);
                #pragma omp parallel for
                for(uint j = 0; j < filledBundle.size(); ++j) {
                    float dr = bundlePath.at(0).r() / radius_old;
                    filledBundle.at(j).addPoint(FAC::FiberPoint(bundlePath.at(0) + MatrixDotVector(rotation, shape.at(j)) * dr, radius));
                }
                //tangent_old = tangent_new;

                for(uint i = 1; i < bundlePath.size(); ++i) {
                   if(i+1 < bundlePath.size() )
                      tangent_new = bundlePath.at(i+1).toQVector3D() - bundlePath.at(i-1).toQVector3D();
                   else
                      tangent_new = bundlePath.at(i).toQVector3D() - bundlePath.at(i-1).toQVector3D();

                    tangent_new.normalize();

                    QMatrix3x3 rotation = RotateFromAToB(tangent_old, tangent_new);
                    float dr = bundlePath.at(i).r() / radius_old;

                    #pragma omp parallel for
                    for(uint j = 0; j < filledBundle.size(); ++j) {
                        shape.at(j) = MatrixDotVector(rotation, shape.at(j));
                        filledBundle.at(j).addPoint(FAC::FiberPoint(bundlePath.at(i) + shape.at(j) * dr, radius));
                    }
                    tangent_old = tangent_new;
                }

                return filledBundle;
            }
        };

        std::vector<FAC::Fiber> CreateFibersFromFiberBundle(FAC::Fiber f, float radius, float distance) {
            // Search minimal radius of bundle
            auto min_r = f.getParts().at(0).r();
            for(FAC::FiberPoint p : f.getParts()) {
                if(p.r() < min_r) min_r = p.r();
            }
            std::vector<QVector3D> circle = PopulateCircle(min_r, distance);
            std::vector<FAC::Fiber> fibers = FormFibersAlongBundle(circle, f, radius, min_r);
            return fibers;
        }

    };
}
