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

// Position (will be passed to the fragment shader)
out vec3 outVertexPosition;

// Normal Matrix (will be passed to the fragment shader)
out vec4 outNormalVector;

uniform mat4 translate,model,mvp;
uniform vec3 vertPos;
void main()
{
	mat4 normalMatrix;

	vec3 newPosition = vertexPosition;

	normalMatrix = transpose(inverse(model));

	gl_Position = translate * vec4(newPosition, 1.0);

	outUV = vertexUV;
	outColor = vertexColor;
	outVertexPosition = vec3(gl_Position[0], gl_Position[1], gl_Position[2]);
	outNormalVector = normalMatrix * vec4(newPosition,1.0);
}
