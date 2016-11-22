#version 330 core

in vec3 vertexColor;
in vec3 fragPos;
in vec3 normal_to_frag;

uniform vec3 lightColor;

out vec4 color;

void main()
{
	float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * vertexColor;
    color = vec4(result, 1.0f);
    
	//color = vec4(vertexColor, 1.0);
} 