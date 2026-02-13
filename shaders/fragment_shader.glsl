#version 410 core

in vec3 vColor;

out vec4 fragColor;

uniform float u_Offset; // uniform variable

void main()
{
   fragColor = vec4(vColor.r - u_Offset, vColor.g, vColor.b, 1.0);
}