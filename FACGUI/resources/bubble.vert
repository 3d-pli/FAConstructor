#version 330 core
layout(location = 0) in vec3 aPosition;

uniform float XMID;
uniform float YMID;
uniform float radius;
uniform mat3 rotation;

uniform bool rgb;

out vec3 color;

#define M_PI 3.1415926535897932384626433832795028841971

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

void main()
{
    vec3 colorPosition = vec3(aPosition.x - XMID, aPosition.y - YMID, aPosition.z);
    colorPosition = rotation * colorPosition;
    if(rgb) {
        color.x = abs((colorPosition.x)) / radius;
        color.y = abs((colorPosition.y)) / radius;
        color.z = abs((colorPosition.z)) / radius;
    } else {
        colorPosition = normalize(colorPosition);
        // Calculate HSV values
        float inclinationValue = asin(colorPosition.z) / M_PI;
        float directionValue = M_PI + atan(colorPosition.y, colorPosition.x);
        float H = 360*abs(directionValue)/M_PI;
        float S = 1-(2*abs(inclinationValue));
        float V = 1.0;
        color = hsv2rgb(vec3(H, S, V));
    }
    gl_Position = vec4(aPosition, 1.0);
}
