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

uniform vec3 ambientLightColor,diffuseLightColor,specularLightColor;
uniform vec3 lightLoc;

void main()
{
	vec3 ambient;

	vec3 diffuse;
	vec3 result;

	vec3 lightPosition;
	vec3 to_light;

	float cosAngle;

	ambient = ambientLightColor;

	lightPosition = lightLoc;
	to_light = lightPosition - outVertexPosition;
	to_light = normalize(to_light);

	cosAngle = max(dot(vec4(to_light,0.0), outNormalVector), 0.0);

	diffuse = diffuseLightColor * cosAngle;

	// Get pixel color of the texture at the current UV coordinate
	// and output it as our final fragment color
	result = ambient + diffuse;
	fragColor =  vec4(result,0) * texture(tex, outUV);
}
