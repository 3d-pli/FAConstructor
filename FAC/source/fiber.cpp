#include <fiber.h>

FAC::Fiber::Fiber(const unsigned long long size)
{
    dataPoints.resize(size);
    this->active = true;
}

FAC::Fiber::Fiber(const FAC::Fiber &fiber) {
    auto parts = fiber.getParts();
    dataPoints.resize(parts.size());
    #pragma omp parallel for
    for(unsigned i = 0; i < parts.size(); ++i) {
        dataPoints.at(i) = FiberPoint(parts.at(i));
    }
    this->active = fiber.isActive();
    this->m_function = fiber.function();
}

FAC::Fiber::Fiber(std::vector<FAC::FiberPoint>& dataPoints) : dataPoints(dataPoints) {
    this->active = true;
}

FAC::Fiber::Fiber(QVector<FAC::FiberPoint>& dataPoints) {
    this->dataPoints = dataPoints.toStdVector();
    this->active = true;
}

bool FAC::Fiber::isActive() const {
    return active;
}

std::vector<FAC::FiberPoint> FAC::Fiber::getParts() const {
    return this->dataPoints;
}

void FAC::Fiber::setFiberPoint(const unsigned at, const FAC::FiberPoint point) {
    this->dataPoints.at(at) = point;
}

void FAC::Fiber::addPoint(FAC::FiberPoint point) {
    this->dataPoints.push_back(point);
}

void FAC::Fiber::setActive(bool active) {
    this->active = active;
}

void FAC::Fiber::translate(QVector3D& dimensions) {
    translate(dimensions.x(), dimensions.y(), dimensions.z());
}

void FAC::Fiber::translate(float x, float y, float z) {
    for(auto point = this->dataPoints.begin(); point != dataPoints.end(); ++point) {
        point->m_x += x;
        point->m_y += y;
        point->m_z += z;
    }
}

void FAC::Fiber::rotate(float x_angle, float y_angle, float z_angle) {
    QMatrix4x4 m;
    m.rotate(x_angle, QVector3D(1, 0, 0));
    m.rotate(y_angle, QVector3D(0, 1, 0));
    m.rotate(z_angle, QVector3D(0, 0, 1));

    for(auto point = this->dataPoints.begin(); point != dataPoints.end(); ++point) {
        QVector3D result(point->x(), point->y(), point->z());
        result = m * result;

        point->m_x = result.x();
        point->m_y = result.y();
        point->m_z = result.z();
    }
}

void FAC::Fiber::thin(unsigned by) {
    for(int i = this->dataPoints.size()-2; i > 1; i-=by) {
        this->dataPoints.erase(this->dataPoints.begin()+i);
    }
}

void FAC::Fiber::setCreatedByFunction(FAC::FiberFunction function) {
    this->m_function = function;
}

FAC::FiberFunction FAC::Fiber::function() const {
    return this->m_function;
}

FAC::Fiber FAC::Fiber::createByFunction(FAC::FiberFunction function, float stepsize) {
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T> expression_t;
    typedef exprtk::parser<T> parser_t;

    T t;

    if(function.max() <= function.min()) {
        throw QString("Calculation not possible. Either x or y has an invalid range.");
    }

    std::array<std::string, 4> functions = {
        function.functionX().toStdString(), function.functionY().toStdString(), function.functionZ().toStdString(),
        function.radius().toStdString()
    };

    // Prepare rotation for every point of the function
    QMatrix4x4 m;
    m.rotate(function.rotX(), 1, 0, 0);
    m.rotate(function.rotY(), 0, 1, 0);
    m.rotate(function.rotZ(), 0, 0, 1);
    QVector3D toTransform(function.transformX(), function.transformY(), function.transformZ());

    Fiber fiber = Fiber();
    fiber.setCreatedByFunction(function);

    unsigned steps = ceil((function.max()-function.min())/stepsize)+1;
    std::vector<float> values(functions.size()*steps);

    bool error = false;
    #pragma omp parallel for private(t)
    for(unsigned i = 0; i < functions.size(); ++i) {
        // Create symbol table and choose "t" as the parameter
        symbol_table_t symbol_table;
        symbol_table.add_constants();
        symbol_table.add_variable("t", t);

        expression_t expression;
        expression.register_symbol_table(symbol_table);

        // Try to parse and evaluate functions. If an error occurs...
        parser_t parser;
        if(!parser.compile(functions.at(i), expression)) {
            error = true;
        } else {
            for(unsigned j = 0; j < steps; ++j) {
                t = function.min() + j * stepsize;
                values.at(i * steps + j) = expression.value();
            }
        }
    }
    // ... throw it here to present an error message to the user.
    if(error) throw QString("Calculation was not possible. Maybe a non-valid variable or misplaced brace was used. Please check your function and try again!");

    // Add additional translation to the fiber.
    #pragma omp parallel for ordered
    for(unsigned i = 0; i < steps; ++i) {
        QVector4D toAdd(values.at(i), values.at(steps+i), values.at(steps*2+i), values.at(steps*3+i));
        toAdd = m * toAdd;
        toAdd += toTransform;
        #pragma omp ordered
        fiber.addPoint(FiberPoint(toAdd));
    }
    return fiber;
}

