#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
uniform int aForward;
uniform float aScala;

out vec4 vertexColor;
out vec2 TexCoord;
void main(){
	gl_Position = vec4(aPos.x * aScala, aPos.y * aForward, aPos.z, 1.f);
	vertexColor = vec4(aColor.xyz, 1.f);
	TexCoord = aTexCoord;
}