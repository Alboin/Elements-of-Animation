#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 vertexColor;
out vec3 fragPos;
out vec3 normal_to_frag;

uniform mat4 MVP;
uniform mat4 model;
uniform vec3 lightPos;

void main()
{
    gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
	fragPos = vec3(model * vec4(position, 1.0f));
	normal_to_frag = normal;
	vertexColor = color;
}