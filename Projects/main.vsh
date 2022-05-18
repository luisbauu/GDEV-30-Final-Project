#version 330

// Vertex position
layout(location = 0) in vec3 vertexPosition;

// Vertex color
layout(location = 1) in vec3 vertexColor;

// Vertex UV coordinate
layout(location = 2) in vec2 vertexUV;

// UV coordinate (will be passed to the fragment shader)
out vec2 outUV;

// Color (will be passed to the fragment shader)
out vec3 outColor;

uniform mat4 translate;

void main()
{
	vec3 newPosition = vertexPosition;

	gl_Position = translate * vec4(newPosition, 1.0);
	outUV = vertexUV;
	outColor = vertexColor;
}
