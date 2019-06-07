#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aInfo;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;

out VS_OUT {
    vec3 position;
    int color;
    int fiber;
    float radius;
    int edgeValue;
} vs_out;

int shallBeRendered() {
    int edgeValue = 000000;
    vec4 part = worldToCamera * modelToWorld * vec4(aPosition, 1.0);
    float maxValue = 3;
    float minValue = -3;

    // Check for boundries
    edgeValue += int(part.x < minValue) << 0;
    edgeValue += int(part.x > maxValue) << 1;
    edgeValue += int(part.y < minValue) << 2;
    edgeValue += int(part.y > maxValue) << 3;
    edgeValue += int(part.z < -1)       << 4;
    edgeValue += int(part.z > 1000)     << 5;

    return edgeValue;
}

void main()
{
    vs_out.position = aPosition;
    vs_out.color = int(aInfo.x);
    vs_out.fiber = int(aInfo.y);
    vs_out.radius = aInfo.z;
    vs_out.edgeValue = shallBeRendered();
}
