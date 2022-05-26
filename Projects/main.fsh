#version 330

// UV-coordinate of the fragment (interpolated by the rasterization stage)
in vec2 outUV;

// Color of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outColor;

// Vertex Position of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outVertexPosition;

// Normal Matrix of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec4 outNormalVector;

// Final color of the fragment that will be rendered on the screen
out vec4 fragColor;

// Texture unit of the texture
uniform sampler2D tex;

uniform vec3 ambientLightColor,diffuseLightColor,specularLightColor, objectSpecularColor;
uniform vec3 lightLoc;
uniform vec3 camLoc;
uniform float shiny;
uniform float time;
void main()
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 result;

	vec3 lightPosition;
	vec3 to_light;

	vec3 camPosition;
	vec3 camDirection;

	vec4 reflection;

	float shininess;
	float spec;
	float cosAngle;

	shininess = shiny;
	ambient = ambientLightColor;

	lightPosition = lightLoc;
	to_light = lightPosition - outVertexPosition;
	to_light = normalize(to_light);

	cosAngle = max(dot(vec4(to_light,0.2), outNormalVector), 0.0);

	diffuse = diffuseLightColor * cosAngle;

	camDirection = camPosition - outVertexPosition;

	reflection = reflect(vec4(-to_light,1.0), outNormalVector);
	camDirection = normalize(camPosition - outVertexPosition);
	spec = pow(max(dot(reflection, vec4(camDirection,0.0)), 0.0), shininess);
	specular = specularLightColor * objectSpecularColor * spec;

	// Get pixel color of the texture at the current UV coordinate
	// and output it as our final fragment color
	result = (sin(time))*(ambient + diffuse + specular);

	fragColor =  vec4(result,0.0) * texture(tex, outUV);
}
