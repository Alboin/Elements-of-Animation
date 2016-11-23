#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 vertexColor;
out vec3 fragPos;
out vec3 normal_to_frag;
out vec3 vertex_pos;

uniform mat4 MVP;
uniform mat4 model;
uniform float glfw_time;

vec3 generate_wave()
{
			float temp = 0.1*sin((glfw_time + position.x + position.z) * 3);
			temp += 0.1*sin(glfw_time*1.3f + position.x * 3);
			temp += 0.1*sin(glfw_time*1.6f + position.y * 4);
			temp /= 2;

			return vec3(position.x, temp, position.z);
}


void main()
{
	vec3 pos = generate_wave();
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z, 1.0);
	vertex_pos = pos;
	fragPos = vec3(model * vec4(position, 1.0f));
	normal_to_frag = normal;
	vertexColor = color;
}