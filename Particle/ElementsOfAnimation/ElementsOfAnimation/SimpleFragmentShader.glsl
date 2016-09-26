#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Ouput data
out vec3 color;

uniform float TIME;

void main(){
  color =  fragmentColor * (sin(TIME)*0.5 + 0.7);
}