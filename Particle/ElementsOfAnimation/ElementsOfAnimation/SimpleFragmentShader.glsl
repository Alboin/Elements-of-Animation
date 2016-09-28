#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Ouput data
out vec3 color;

uniform float TIME;

void main(){
  color =  fragmentColor; //mod(fragmentColor + TIME/1, 1);
}