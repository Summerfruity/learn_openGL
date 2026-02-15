#version 410 core

layout(location = 3) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 u_ModelMatrix;

out vec3 vColor;

void main()
{
   vec4 newPosition = u_ModelMatrix * vec4(position, 1.0f);

   gl_Position = vec4(newPosition.x,newPosition.y, newPosition.z, 1.0f);
   vColor = color;
}