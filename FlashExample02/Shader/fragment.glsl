#version 330
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 ourColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main(){

	FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(TexCoord.x * 2, TexCoord.y * 2)), mixValue);
}