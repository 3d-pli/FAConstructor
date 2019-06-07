#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 66) out;

#define M_PI 3.1415926535897932384626433832795028841971

in VS_OUT {
    vec3 position;
    int color;
    int fiber;
    float radius;
    int edgeValue;
} gs_in[];

out vec3 g_color;
out vec4 g_normal;
out vec4 g_position;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform int corners;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy, vec2(a,b));
    highp float sn= mod(dt,M_PI);
    return fract(sin(sn) * c);
}

vec3 hsv2rgb(vec3 c) {
    float H = c.x; // [0,360]
    float S = c.y; // [0,1]
    float V = c.z; // [0,1]

    if( H >= 360) H -= 360;
    if( H <  0  ) H += 360;

    int hi= int(floor(H/60.));
    float f = H/60. -hi;

    float p = V*(1-S);
    float q = V*(1-S*f);
    float t = V*(1-S*(1-f));

    vec3 rgb;

    switch(hi)
    {
    case 0: case 6:
         rgb = vec3(V,t,p);
         break;
    case 1:
         rgb = vec3(q,V,p);
         break;
    case 2:
         rgb = vec3(p,V,t);
         break;
    case 3:
         rgb = vec3(p,q,V);
         break;
    case 4:
         rgb = vec3(t,p,V);
         break;
    case 5:
         rgb = vec3(V,p,q);
         break;
    }
    return rgb;
}

void setColor(int i) {
    switch(gs_in[i].color) {
    case 0:
        g_color = gs_in[i+1].position - gs_in[i].position;
        g_color = normalize(g_color);
        g_color.x = g_color.x < 0 ? -g_color.x : g_color.x;
        g_color.y = g_color.y < 0 ? -g_color.y : g_color.y;
        g_color.z = g_color.z < 0 ? -g_color.z : g_color.z;
        return;
    case 1:
        g_color = gs_in[i+1].position - gs_in[i].position;
        g_color = normalize(g_color);
        // Calculate HSV values
        float inclinationValue = (asin(g_color.z)) / M_PI;
        float directionValue = M_PI + atan(g_color.y, g_color.x);
        float H = 360*abs(directionValue)/M_PI;
        float S = 1-(2*abs(inclinationValue));
        float V = 1.0;
        g_color = hsv2rgb(vec3(H, S, V));
        return;
    case 2:
        g_color = vec3(rand(vec2(gs_in[i].fiber+1, (gs_in[i].fiber+1)*(gs_in[i].fiber+1))), rand(vec2((gs_in[i].fiber+1)*(gs_in[i].fiber+1), gs_in[i].fiber+1)), rand(vec2(gs_in[i].fiber+1, gs_in[i].fiber+1)));
        return;
    default:
        g_color = vec3(0.75, 0.75, 0.75);
        return;
    }
}

void draw(int i) {
    vec3 _begin = gs_in[i].position;
    vec4 begin = vec4(gs_in[i].position, 0.0);
    vec3 _end = gs_in[i+1].position;
    vec4 end = vec4(_end, 0.0);
    mat4 worldToCamera = worldToCamera;
    mat4 modelToWorld = modelToWorld;
    mat4 cylinderRotation = mat4(1.0);

    vec3 vecOrientation = _end - _begin;
    vecOrientation = normalize(vecOrientation);

    vec3 cylinderOrientation = vec3(0.0, 1.0, 0.0);
    if(vecOrientation.y < 0) {
        vecOrientation = _begin - _end;
        vecOrientation = normalize(vecOrientation);
        vec4 tmp = begin;
        begin = end;
        end = tmp;
    }
    float c = dot(vecOrientation, cylinderOrientation);
    vec3 d = vec3(cross(vecOrientation, cylinderOrientation)) ;
    float s = length(d);

    mat4 a_x = mat4(0.0,    -d.z,   d.y,    0,
                    d.z,    0,      -d.x,   0,
                    -d.y,   d.x,    0,      0,
                    0,      0,      0,      0);
    cylinderRotation = mat4(1.0) + s * a_x + (1-c) * (a_x * a_x);

    float radius_begin = gs_in[i].radius;
    float radius_end = gs_in[i+1].radius;

    for(int j=0; j<corners; j++) {
        float theta = j*2*M_PI/corners;
        float nextTheta = (j+1)*2*M_PI/corners;

        g_position = worldToCamera * modelToWorld * (end + cylinderRotation * (vec4(0, 0, 0, 1)));
        g_normal = cylinderRotation * vec4(cylinderOrientation * normalize(vec3(0, 1, 0)), 1.0);
        gl_Position = g_position;
        EmitVertex();
        g_position = worldToCamera * modelToWorld * (end + cylinderRotation * (vec4(radius_end*cos(theta), 0, radius_end*sin(theta), 1)));
        g_normal = cylinderRotation * vec4(normalize(vec3(radius_end*cos(theta), 0, radius_end*sin(theta))), 1.0);
        gl_Position = g_position;
        EmitVertex();
        g_position = worldToCamera * modelToWorld * (end + cylinderRotation * (vec4(radius_end*cos(nextTheta), 0, radius_end*sin(nextTheta), 1)));
        g_normal = cylinderRotation * vec4(normalize(vec3(radius_end*cos(nextTheta), 0, radius_end*sin(nextTheta))), 1.0);
        gl_Position = g_position;
        EmitVertex();
        g_position = worldToCamera * modelToWorld * (begin + cylinderRotation * (vec4(radius_begin*cos(theta), 0, radius_begin*sin(theta), 1)));
        g_normal = cylinderRotation * vec4(normalize(vec3(radius_begin*cos(theta), 0, radius_begin*sin(theta))), 1.0);
        gl_Position = g_position;
        EmitVertex();
        g_position = worldToCamera * modelToWorld * (begin + cylinderRotation * (vec4(radius_begin*cos(nextTheta), 0, radius_begin*sin(nextTheta), 1)));
        g_normal = cylinderRotation * vec4(normalize(vec3(radius_begin*cos(nextTheta), 0, radius_begin*sin(nextTheta))), 1.0);
        gl_Position = g_position;
        EmitVertex();
        g_position = worldToCamera * modelToWorld * (begin + cylinderRotation * (vec4(0, 0, 0, 1)));
        g_normal = cylinderRotation * vec4(cylinderOrientation * normalize(vec3(0, 1, 0)), 1.0);
        gl_Position = g_position;
        EmitVertex();
    }
}

void main() {
    if(gs_in[0].edgeValue == 0000 || gs_in[1].edgeValue == 0000 || (gs_in[0].edgeValue & gs_in[1].edgeValue) == 0000) {
        setColor(0);
        draw(0);
    }
    EndPrimitive();
}
