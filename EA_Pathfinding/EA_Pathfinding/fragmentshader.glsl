#version 330 core

in vec3 vertexColor;
in vec3 fragPos;
in vec3 normal_to_frag;
in vec3 vertex_pos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float glfw_time;
uniform int procedural;
uniform float scale_length;
uniform float scale_height;

out vec4 color;

vec3 calc_normal()
{
	float step = 0.1;		
	float temp = scale_height*0.1*sin((glfw_time + (vertex_pos.x + step) + vertex_pos.z) * 3 * scale_length);
	temp += scale_height*0.1*sin(glfw_time*1.3f + (vertex_pos.x + step) * 3 * scale_length);
	temp += scale_height*0.1*sin(glfw_time*1.6f + vertex_pos.y * 4 * scale_length);
	temp /= 2;

	vec3 B = vec3((vertex_pos.x + step), temp, vertex_pos.z);
	
	temp = scale_height*0.1*sin((glfw_time + vertex_pos.x + (vertex_pos.z + step)) * 3 * scale_length);
	temp += scale_height*0.1*sin(glfw_time*1.3f + vertex_pos.x * 3 * scale_length);
	temp += scale_height*0.1*sin(glfw_time*1.6f + vertex_pos.y * 4 * scale_length);
	temp /= 2;

	vec3 C = vec3(vertex_pos.x, temp, (vertex_pos.z + step));
	
	return normalize(cross(C - vertex_pos,B - vertex_pos));	
}

void main()
{
	vec3 norm = normal_to_frag;
	if(procedural == 1)
	{
		norm = calc_normal();
	}

	vec3 lightDir = normalize(lightPos - fragPos);

	//Compute diffuse component
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//Compute ambient component
	float ambientStrength = 0.15f;
    vec3 ambient = ambientStrength * lightColor;

	//Compute specular component
	float specularStrength = 3.0f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	vec3 specular = specularStrength * spec * lightColor; 

	vec3 result = (ambient + diffuse + specular) * vertexColor;

    color = vec4(result, 1.0f);
    
	//color = vec4(vertexColor, 1.0);
} 