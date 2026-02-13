#version 410 core

layout(location = 3) in vec3 position;
layout(location = 1) in vec3 color;

uniform float u_Offset; // uniform variable

out vec3 vColor;

void main()
{
   gl_Position = vec4(position.x,position.y + u_Offset, position.z, 1.0);
   vColor = color;
}