#version 330 core
out vec4 fColor;
in vec3 g_color;
in vec4 g_normal;
in vec4 g_position;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform vec3 cameraPos;
uniform vec3 lightPos;

void main()
{
    // Calculate depth to show objects behind others
    vec4 pos = g_position;
    //gl_FragDepth = clamp(pos.z / 100., 0, 1);

    // Lighting
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 normal = g_normal.xyz;

    // Diffusion
    vec3 surfaceToLight = normalize(lightPos - pos.xyz);
    float diffusionStrength = dot(normal, surfaceToLight);
    diffusionStrength = clamp(diffusionStrength, 0, 1);
    vec4 diffusion = lightColor * diffusionStrength;

    // Ambient light
    float ambientStrength = 0.3;
    vec4 ambient = lightColor * ambientStrength;

    // Specular
    vec3 viewDir = normalize(cameraPos - g_position.xyz);
    vec3 reflectDir = reflect(-normalize(lightPos), normal);
    float spec = pow(clamp(dot(viewDir, reflectDir), 0, 1), 2);
    vec4 specular = spec * lightColor;

    fColor = (ambient+diffusion+specular) * vec4(g_color, 1.0);
}
