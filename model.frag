#version 330 core

uniform sampler2D diffuse_map;

in vec2 TexCoords;

out vec4 color;

float LinearizeDepth(float depth)
{
    float near = 0.1;
    float far = 100.0;
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near) / (far + near - z * (far - near));
    //return far * near / (((near - far) * z + far) * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z);
    color = vec4(vec3(depth), 1.0f);
    //color = vec4(vec3(texture(diffuse_map, TexCoords)),1.0f);
}


