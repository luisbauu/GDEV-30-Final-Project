// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>   

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ---------------
// Function declarations
// ---------------

/// <summary>
/// Creates a shader program based on the provided file paths for the vertex and fragment shaders.
/// </summary>
/// <param name="vertexShaderFilePath">Vertex shader file path</param>
/// <param name="fragmentShaderFilePath">Fragment shader file path</param>
/// <returns>OpenGL handle to the created shader program</returns>
GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the path to the file containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderFilePath">Path to the file containing the shader source</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the string containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderSource">Shader source string</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource);

/// <summary>
/// Function for handling the event when the size of the framebuffer changed.
/// </summary>
/// <param name="window">Reference to the window</param>
/// <param name="width">New width</param>
/// <param name="height">New height</param>
void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height);

/// <summary>
/// Struct containing data about a vertex
/// </summary>
struct Vertex
{
	GLfloat x, y, z;	// Position
	GLubyte r, g, b;	// Color
	GLfloat u, v;		// UV coordinates
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  800.0f / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int windowWidth = 800;
int windowHeight = 800;

float globalSpeed = 10.0f;

/// <summary>
/// Main function.
/// </summary>
/// <returns>An integer indicating whether the program ended successfully or not.
/// A value of 0 indicates the program ended succesfully, while a non-zero value indicates
/// something wrong happened during execution.</returns>
int main()
{
	// Initialize GLFW
	int glfwInitStatus = glfwInit();
	if (glfwInitStatus == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return 1;
	}

	// Tell GLFW that we prefer to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW that we prefer to use the modern OpenGL
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tell GLFW to create a window

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Textures", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Tell GLFW to use the OpenGL context that was assigned to the window that we just created
	glfwMakeContextCurrent(window);

	// Register the callback function that handles when the framebuffer size has changed
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	// Tell GLAD to load the OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return 1;
	}

	// tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// --- Vertex specification ---
	
	// Set up the data for each vertex of the triangle
	Vertex vertices[24];
	//______________________________________FACING UP	
	vertices[0].x = -1.0f;	vertices[0].y = -1.0f;	vertices[0].z = -1.0;
	vertices[0].r = 255;	vertices[0].g = 255;	vertices[0].b = 255;
	vertices[0].u = 0.0f;	vertices[0].v = 0.0f;

	vertices[1].x = 1.0f;	vertices[1].y = -1.0f;	vertices[1].z = -1.0f;
	vertices[1].r = 255;	vertices[1].g = 255;	vertices[1].b = 255;
	vertices[1].u = 1.0f;	vertices[1].v = 0.0f;

	vertices[2].x = -1.0f;	vertices[2].y = 1.0f;	vertices[2].z = -1.0f;
	vertices[2].r = 255;	vertices[2].g = 255;	vertices[2].b = 255;
	vertices[2].u = 0.0f;	vertices[2].v = 1.0f;

	vertices[3].x = 1.0f;	vertices[3].y = 1.0f;	vertices[3].z = -1.0f;
	vertices[3].r = 255;	vertices[3].g = 255;	vertices[3].b = 255;
	vertices[3].u = 1.0f;	vertices[3].v = 1.0f;
	//______________________________________ FRONT FACING FORWARD
	vertices[4].x = 1.0f;	vertices[4].y = 1.0f;	vertices[4].z = 1.0f;
	vertices[4].r = 255;	vertices[4].g = 255;		vertices[4].b = 255;
	vertices[4].u = 0.0f;	vertices[4].v = 1.0f;

	vertices[5].x = 1.0f;	vertices[5].y = -1.0f;	vertices[5].z = 1.0f;
	vertices[5].r = 255;	vertices[5].g = 255;	vertices[5].b = 255;
	vertices[5].u = 0.0f;	vertices[5].v = 0.0f;

	vertices[6].x = -1.0f;	vertices[6].y = 1.0f;	vertices[6].z = 1.0f;
	vertices[6].r = 255;	vertices[6].g = 255;		vertices[6].b = 255;
	vertices[6].u = 1.0f;	vertices[6].v = 1.0f;

	vertices[7].x = -1.0f;	vertices[7].y = -1.0f;	vertices[7].z = 1.0f;
	vertices[7].r = 255;	vertices[7].g = 255;	vertices[7].b = 255;
	vertices[7].u = 1.0f;	vertices[7].v = 0.0f;
//______________________________________
	vertices[8].x = -1.0f;	vertices[8].y = 1.0f;	vertices[8].z = -1.0f;
	vertices[8].r = 255;	vertices[8].g = 255;		vertices[8].b = 255;
	vertices[8].u = 0.0f;	vertices[8].v = 0.0f;

	vertices[9].x = -1.0f;	vertices[9].y = 1.0f;	vertices[9].z = 1.0f;
	vertices[9].r = 255;	vertices[9].g = 255;	vertices[9].b = 255;
	vertices[9].u = 0.0f;	vertices[9].v = 1.0f;

	vertices[10].x = 1.0f;	vertices[10].y = 1.0f;	vertices[10].z = -1.0f;
	vertices[10].r = 255;	vertices[10].g = 255;		vertices[10].b = 255;
	vertices[10].u = 1.0f;	vertices[10].v = 0.0f;

	vertices[11].x = 1.0f;	vertices[11].y = 1.0f;	vertices[11].z = 1.0f;
	vertices[11].r = 255;	vertices[11].g = 255;	vertices[11].b = 255;
	vertices[11].u = 1.0f;	vertices[11].v = 1.0f;
//______________________________________
	vertices[12].x = -1.0f;	vertices[12].y = -1.0f;	vertices[12].z = -1.0f;
	vertices[12].r = 255;	vertices[12].g = 255;		vertices[12].b = 255;
	vertices[12].u = 0.0f;	vertices[12].v = 0.0f;

	vertices[13].x = -1.0f;	vertices[13].y = -1.0f;	vertices[13].z = 1.0f;
	vertices[13].r = 255;	vertices[13].g = 255;	vertices[13].b = 255;
	vertices[13].u = 0.0f;	vertices[13].v = 1.0f;

	vertices[14].x = 1.0f;	vertices[14].y = -1.0f;	vertices[14].z = -1.0f;
	vertices[14].r = 255;	vertices[14].g = 255;		vertices[14].b = 255;
	vertices[14].u = 1.0f;	vertices[14].v = 0.0f;

	vertices[15].x = 1.0f;	vertices[15].y = -1.0f;	vertices[15].z = 1.0f;
	vertices[15].r = 255;	vertices[15].g = 255;	vertices[15].b = 255;
	vertices[15].u = 1.0f;	vertices[15].v = 1.0f;
//______________________________________
	vertices[16].x = -1.0f;	vertices[16].y = -1.0f;	vertices[16].z = 1.0f;
	vertices[16].r = 255;	vertices[16].g = 255;	vertices[16].b = 255;
	vertices[16].u = 0.0f;	vertices[16].v = 0.0f;

	vertices[17].x = -1.0f;	vertices[17].y = 1.0f;	vertices[17].z = 1.0f;
	vertices[17].r = 255;	vertices[17].g = 255;	vertices[17].b = 255;
	vertices[17].u = 0.0f;	vertices[17].v = 1.0f;

	vertices[18].x = -1.0f;	vertices[18].y = -1.0f;	vertices[18].z = -1.0f;
	vertices[18].r = 255;	vertices[18].g = 255;	vertices[18].b = 255;
	vertices[18].u = 1.0f;	vertices[18].v = 0.0f;

	vertices[19].x = -1.0f;	vertices[19].y = 1.0f;	vertices[19].z = -1.0f;
	vertices[19].r = 255;	vertices[19].g = 255;	vertices[19].b = 255;
	vertices[19].u = 1.0f;	vertices[19].v = 1.0f;
//______________________________________
	vertices[20].x = 1.0f;	vertices[20].y = -1.0f;	vertices[20].z = -1.0f;
	vertices[20].r = 255;	vertices[20].g = 255;	vertices[20].b = 255;
	vertices[20].u = 0.0f;	vertices[20].v = 0.0f;

	vertices[21].x = 1.0f;	vertices[21].y = 1.0f;	vertices[21].z = -1.0f;
	vertices[21].r = 255;	vertices[21].g = 255;	vertices[21].b = 255;
	vertices[21].u = 0.0f;	vertices[21].v = 1.0f;

	vertices[22].x = 1.0f;	vertices[22].y = -1.0f;	vertices[22].z = 1.0f;
	vertices[22].r = 255;	vertices[22].g = 255;	vertices[22].b = 255;
	vertices[22].u = 1.0f;	vertices[22].v = 0.0f;

	vertices[23].x = 1.0f;	vertices[23].y = 1.0f;	vertices[23].z = 1.0f;
	vertices[23].r = 255;	vertices[23].g = 255;	vertices[23].b = 255;
	vertices[23].u = 1.0f;	vertices[23].v = 1.0f;
//______________________________________
	vertices[24].x = 1.0f;	vertices[24].y = 1.0f;	vertices[24].z = 0.0f;
	vertices[24].r = 255;	vertices[24].g = 255;	vertices[24].b = 255;
	vertices[24].u = 0.0f;	vertices[24].v = 0.0f;

	vertices[25].x = 1.0f;	vertices[25].y = 1.0f;	vertices[25].z = 1.0f;
	vertices[25].r = 255;	vertices[25].g = 255;	vertices[25].b = 255;
	vertices[25].u = 0.0f;	vertices[25].v = 1.0f;

	vertices[26].x = 1.0f;	vertices[26].y = 0.0f;	vertices[26].z = 1.0f;
	vertices[26].r = 255;	vertices[26].g = 255;	vertices[26].b = 255;
	vertices[26].u = 1.0f;	vertices[26].v = 0.0f;

	vertices[27].x = 1.0f;	vertices[27].y = 0.0f;	vertices[27].z = 0.0f;
	vertices[27].r = 255;	vertices[27].g = 255;	vertices[27].b = 255;
	vertices[27].u = 1.0f;	vertices[27].v = 1.0f;

	// Create a vertex buffer object (VBO), and upload our vertices data to the VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object that contains data on how to map vertex attributes
	// (e.g., position, color) to vertex shader properties.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	// Vertex attribute 0 - Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

	// Vertex attribute 1 - Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

	// Vertex attribute 2 - UV coordinate
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));

	glBindVertexArray(0);

	// Create a shader program
	GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

	// Tell OpenGL the dimensions of the region where stuff will be drawn.
	// For now, tell OpenGL to use the whole screen
	glViewport(0, 0, windowWidth, windowHeight);

	// Create a variable that will contain the ID for our texture,
	// and use glGenTextures() to generate the texture itself
	GLuint tex;
	glGenTextures(1, &tex);

	// --- Load our image using stb_image ---

	// Im image-space (pixels), (0, 0) is the upper-left corner of the image
	// However, in u-v coordinates, (0, 0) is the lower-left corner of the image
	// This means that the image will appear upside-down when we use the image data as is
	// This function tells stbi to flip the image vertically so that it is not upside-down when we use it
	stbi_set_flip_vertically_on_load(true);

	// 'imageWidth' and imageHeight will contain the width and height of the loaded image respectively
	int imageWidth, imageHeight, numChannels;

	// Read the image data and store it in an unsigned char array
	unsigned char* imageData = stbi_load("pepehappy.jpg", &imageWidth, &imageHeight, &numChannels, 0);
	// unsigned char* imageData = stbi_load("pepesad.jpg", &imageWidth, &imageHeight, &numChannels, 0);


	//Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load image" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);

	glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 diffuseColor = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 specularColor = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 objectSpecular = glm::vec3(0.4f, 0.4f, 0.4f);

	glm::vec3 lightLocation = glm::vec3(0.0f, 0.0f, 0.0f);

	float specShine = 0.3;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

		//BG COLOR RGBA FORMAT
		glClearColor(245.0f/255.0f,245.0f/255.0f,220.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), (float)imageWidth / (float)imageHeight, 0.1f, 100.0f);

		// glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  //BACK FACING FORWARD
		// glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);  //FRONT FACING FORWARD
		// glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);  //CEILING
		// glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); //FLOOR
		// glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); //LEFT WALL
		// glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); //RIGHT WALL
		
		glUseProgram(program);
		glBindVertexArray(vao);
		//Ambient Lighting

		GLint ambientLightingUniform = glGetUniformLocation(program, "ambientLightColor");
		glUniform3fv(ambientLightingUniform, 1, glm::value_ptr(ambientColor));
		
		GLint diffuseLightingUniform = glGetUniformLocation(program, "diffuseLightColor");
		glUniform3fv(diffuseLightingUniform, 1, glm::value_ptr(diffuseColor));

		GLint specularLightingUniform = glGetUniformLocation(program, "specularLightColor");
		glUniform3fv(specularLightingUniform, 1, glm::value_ptr(specularColor));
		
		GLint objectSpecularUniform = glGetUniformLocation(program, "objectSpecularColor");
		glUniform3fv(objectSpecularUniform, 1, glm::value_ptr(objectSpecular));

		GLint lightPositionUniform = glGetUniformLocation(program, "lightLoc");
		glUniform3fv(lightPositionUniform, 1, glm::value_ptr(lightLocation));

		GLint shinyUniform = glGetUniformLocation(program, "shiny");
		glUniform1f(shinyUniform, specShine);

		GLint cameraPositionUniform = glGetUniformLocation(program, "camLoc");
		glUniform3fv(cameraPositionUniform, 1, glm::value_ptr(cameraPos));

		// FLOOR
		glm::vec3 floorArray[101];
		int floatCount = 0;
		for(int i = 0; i < 10; i++){
			for(int j = 0 ; j < 10; j++)
			{
				floorArray[floatCount] = glm::vec3((-2.0 * i),  0.0f, (-2.0 * j));
				floatCount++;
			}
		}

		glm::mat4 mat = glm::mat4(1.0f);

		for(int i = 0; i < 101; i++)
		{
			glm::mat4 mat = glm::mat4(1.0f);
			mat = glm::scale(mat, glm::vec3(1.0f, 1.0f, 1.0f));
			mat = glm::translate(mat, floorArray[i]);

			glm::mat4 modelMatrix = projectionMatrix * viewMatrix * mat;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
		}

		

		//WALLS

		glm::vec3 rightWallArray[11];
		for(int i = 0; i < 10; i++){
			rightWallArray[i] = glm::vec3( 0.0f,  0.0f,(-2.0 * i));
		}

		for(int i = 0; i < 10; i++)
		{
			glm::mat4 mat2 = glm::mat4(1.0f);
			mat2 = glm::scale(mat2, glm::vec3(1.0f, 1.0f, 1.0f));
			mat2 = glm::translate(mat2,rightWallArray[i]);


			glm::mat4 modelMatrix2 = projectionMatrix * viewMatrix * mat2;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat2));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix2));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix2));

			glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
		}

		glm::vec3 leftWallArray[11];
		for(int i = 0; i < 10; i++){
			leftWallArray[i] = glm::vec3( -18.0f,  0.0f,(-2.0 * i));
		}

		for(int i = 0; i < 10; i++)
		{
			glm::mat4 mat3 = glm::mat4(1.0f);
			mat3 = glm::scale(mat3, glm::vec3(1.0f, 1.0f, 1.0f));
			mat3 = glm::translate(mat3,leftWallArray[i]);

			glm::mat4 modelMatrix3 = projectionMatrix * viewMatrix * mat3;
			
			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat3));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix3));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix3));

			glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
		}

		glm::vec3 frontWallArray[11];
		for(int i = 0; i < 10; i++){
			frontWallArray[i] = glm::vec3( (-2.0 * i),  0.0f, 0.0f);
		}

		for(int i = 0; i < 10; i++)
		{
			glm::mat4 mat4 = glm::mat4(1.0f);
			mat4 = glm::scale(mat4, glm::vec3(1.0f, 1.0f, 1.0f));
			mat4 = glm::translate(mat4,frontWallArray[i]);

			glm::mat4 modelMatrix4 = projectionMatrix * viewMatrix * mat4;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat4));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix4));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix4));

			glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
		}

		glm::vec3 backWallArray[11];
		for(int i = 0; i < 10; i++){
			backWallArray[i] = glm::vec3((-2.0 * i),  0.0f, -18.0f);
		}

		for(int i = 0; i < 10; i++)
		{
			glm::mat4 mat5 = glm::mat4(1.0f);
			mat5 = glm::scale(mat5, glm::vec3(1.0f, 1.0f, 1.0f));
			mat5 = glm::translate(mat5,backWallArray[i]);

			glm::mat4 modelMatrix5 = projectionMatrix * viewMatrix * mat5;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat5));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix5));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix5));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glm::vec3 backWallMazeArray[46] = {
			//FIRST ROW
			glm::vec3(-0.0f, 0.0f,-2.0f),
			glm::vec3(-14.0f, 0.0f,-2.0f),
			glm::vec3(-16.0f, 0.0f,-2.0f),
			glm::vec3(-18.0f, 0.0f,-2.0f),


			glm::vec3(-2.0f, 0.0f,-4.0f),
			glm::vec3(-6.0f, 0.0f,-4.0f),
			glm::vec3(-8.0f,0.0f,-4.0f),
			glm::vec3(-16.0f,0.0f,-4.0f),

			glm::vec3(-4.0f,0.0f,-6.0f),
			glm::vec3(-6.0f,0.0f,-6.0f),
			glm::vec3(-12.0f,0.0f,-6.0f),
			glm::vec3(-14.0f,0.0f,-6.0f),

			glm::vec3(-2.0f, 0.0f, -8.0f),
			glm::vec3(-12.0f,0.0f, -8.0f),
			glm::vec3(-14.0f,0.0f, -8.0f),
			glm::vec3(-16.0f,0.0f, -8.0f),

			glm::vec3(-2.0f,0.0f,-10.0f),

			glm::vec3(0.0f,0.0f,-12.0f),
			glm::vec3(-2.0f,0.0f,-12.0f),
			glm::vec3(-4.0f,0.0f,-12.0f),
			glm::vec3(-14.0f,0.0f,-12.0f),

			glm::vec3(-2.0f,0.0f,-14.0f),
			glm::vec3(-4.0f,0.0f,-14.0f),
			glm::vec3(-10.0f,0.0f,-14.0f),
			glm::vec3(-12.0f,0.0f,-14.0f),
			glm::vec3(-14.0f,0.0f,-14.0f),
			glm::vec3(-16.0f,0.0f,-14.0f),

			glm::vec3(-2.0f,0.0f,-16.0f),
			glm::vec3(-4.0f,0.0f,-16.0f),
			glm::vec3(-8.0f,0.0f,-16.0f),
			glm::vec3(-10.0f,0.0f,-16.0f),
			glm::vec3(-12.0f,0.0f,-16.0f),
			glm::vec3(-14.0f,0.0f,-16.0f),
			glm::vec3(-16.0f,0.0f,-16.0f),


			glm::vec3(-0.0f,0.0f,-18.0f),
			glm::vec3(-12.0f,0.0f,-18.0f),
			glm::vec3(-14.0f,0.0f,-18.0f),
		};

		for(int i = 0; i < 46; i++)
		{
			glm::mat4 mat6 = glm::mat4(1.0f);
			mat6 = glm::scale(mat6, glm::vec3(1.0f, 1.0f, 1.0f));
			mat6 = glm::translate(mat6,backWallMazeArray[i]);

			glm::mat4 modelMatrix6= projectionMatrix * viewMatrix * mat6;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat6));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix6));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix6));

			glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
		}

		glm::vec3 sideWallMazeArray[41] = {
			glm::vec3(-2.0f,0.0f, -4.0f),
			glm::vec3(-2.0f,0.0f, -6.0f),

			glm::vec3(-4.0f,0.0f, 0.0f),
			glm::vec3(-4.0f,0.0f, -2.0f),
			glm::vec3(-4.0f,0.0f, -8.0f),
			glm::vec3(-4.0f,0.0f, -16.0f),
			glm::vec3(-4.0f,0.0f, -18.0f),

			glm::vec3(-6.0f,0.0f, -2.0f),
			glm::vec3(-6.0f,0.0f, -4.0f),
			glm::vec3(-6.0f,0.0f, -8.0f),
			glm::vec3(-6.0f,0.0f, -10.0f),
			glm::vec3(-6.0f,0.0f, -16.0f),

			glm::vec3(-8.0f,0.0f, 0.0f),
			glm::vec3(-8.0f,0.0f, -6.0f),
			glm::vec3(-8.0f,0.0f, -8.0f),
			glm::vec3(-8.0f,0.0f, -10.0f),
			glm::vec3(-8.0f,0.0f, -12.0f),
			glm::vec3(-8.0f,0.0f, -14.0f),
			glm::vec3(-8.0f,0.0f, -16.0f),
			glm::vec3(-8.0f,0.0f, -18.0f),

			glm::vec3(-10.0f,0.0f, -2.0f),
			glm::vec3(-10.0f,0.0f, -10.0f),
			glm::vec3(-10.0f,0.0f, -12.0f),
			glm::vec3(-10.0f,0.0f, -18.0f),

			glm::vec3(-12.0f,0.0f, -2.0f),
			glm::vec3(-12.0f,0.0f, -4.0f),
			glm::vec3(-12.0f,0.0f, -12.0f),
			glm::vec3(-12.0f,0.0f, -14.0f),

			glm::vec3(-14.0f,0.0f, -8.0f),

			glm::vec3(-18.0f,0.0f, -6.0f),
			glm::vec3(-18.0f,0.0f, -8.0f),
			glm::vec3(-18.0f,0.0f, -10.0f),
			glm::vec3(-18.0f,0.0f, -12.0f),
			glm::vec3(-18.0f,0.0f, -14.0f),
			glm::vec3(-18.0f,0.0f, -18.0f),
		};

		for(int i = 0; i < 41; i++)
		{
			glm::mat4 mat7 = glm::mat4(1.0f);
			mat7 = glm::scale(mat7, glm::vec3(1.0f, 1.0f, 1.0f));
			mat7 = glm::translate(mat7, sideWallMazeArray[i]);

			glm::mat4 modelMatrix7 = projectionMatrix * viewMatrix * mat7;

			GLint modelMatrixUniform = glGetUniformLocation(program, "model");
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(mat7));

			GLint mvpUniform = glGetUniformLocation(program, "mvp");
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix7));

			GLint translateuniformLocation = glGetUniformLocation(program, "translate");
			glUniformMatrix4fv(translateuniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix7));

			glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
		}


		// "Unuse" the vertex array object
		glBindVertexArray(0);

		// Tell GLFW to swap the screen buffer with the offscreen buffer
		glfwSwapBuffers(window);

		// Tell GLFW to process window events (e.g., input events, window closed events, etc.)
		glfwPollEvents();
	}

	// --- Cleanup ---

	// Make sure to delete the shader program
	glDeleteProgram(program);

	// Delete the VBO that contains our vertices
	glDeleteBuffers(1, &vbo);

	// Delete the vertex array object
	glDeleteVertexArrays(1, &vao);

	// Remember to tell GLFW to clean itself up before exiting the application
	glfwTerminate();

	return 0;
}

/// <summary>
/// Creates a shader program based on the provided file paths for the vertex and fragment shaders.
/// </summary>
/// <param name="vertexShaderFilePath">Vertex shader file path</param>
/// <param name="fragmentShaderFilePath">Fragment shader file path</param>
/// <returns>OpenGL handle to the created shader program</returns>
GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
	GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	// Check shader program link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "program link error: " << infoLog << std::endl;
	}

	return program;
}

/// <summary>
/// Creates a shader based on the provided shader type and the path to the file containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderFilePath">Path to the file containing the shader source</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath)
{
	std::ifstream shaderFile(shaderFilePath);
	if (shaderFile.fail())
	{
		std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
		return 0;
	}

	std::string shaderSource;
	std::string temp;
	while (std::getline(shaderFile, temp))
	{
		shaderSource += temp + "\n";
	}
	shaderFile.close();

	return CreateShaderFromSource(shaderType, shaderSource);
}

/// <summary>
/// Creates a shader based on the provided shader type and the string containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderSource">Shader source string</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char* shaderSourceCStr = shaderSource.c_str();
	GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
	glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
	glCompileShader(shader);

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "shader compilation error: " << infoLog << std::endl;
	}

	return shader;
}

/// <summary>
/// Function for handling the event when the size of the framebuffer changed.
/// </summary>
/// <param name="window">Reference to the window</param>
/// <param name="width">New width</param>
/// <param name="height">New height</param>
void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
	// Whenever the size of the framebuffer changed (due to window resizing, etc.),
	// update the dimensions of the region to the new size
	glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(globalSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);		
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
