#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec4 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
    mat4 normalMatrix = mat4(mat3(transpose(inverse(view * model))));
    vs_out.normal = normalize(projection * view * normalMatrix * vec4(aNormal, 0.0));
}