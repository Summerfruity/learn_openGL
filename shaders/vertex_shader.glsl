#version 410 core

layout(location = 3) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection;

out vec3 vColor;

void main()
{
   vec4 newPosition = u_Projection * u_ModelMatrix * vec4(position, 1.0f);

   gl_Position = newPosition; // 将顶点位置传递给固定功能管线，进行后续的裁剪、视口变换等处理 
   vColor = color;
}