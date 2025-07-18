#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform int aForward;

out vec4 vertexColor;
void main(){
	gl_Position = vec4(aPos.x, aPos.y * aForward, aPos.z, 1.f);
	vertexColor = vec4(aColor.xyz, 1.f);
}