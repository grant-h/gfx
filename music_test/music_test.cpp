// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <memory.h>

#include <thread>
#include <chrono>

// Include GLEW
#include <GL/glew.h>

#include <bass.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/plyloader.hpp>
#include <common/vboindexer.hpp>

// Object class, vertex, and point
#include <common/Object.h>
#include <common/tga.h>
#include <common/util.h>

// Extra helpers

/* ###################### Structures and Typedefs ###################### */

struct Viewport {
	const char * name;

	float x, y;
	float width, height;
	float bgColor_RGB[3];

	glm::vec3 cameraPos;
	glm::vec3 center;
	glm::mat4 view;
	glm::mat4 projection;
};

/* ###################### Prototypes ###################### */

static std::string getResource(const char * path);
static std::string getResource(std::string path);
static std::string getShader(const char * path);

int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex Vertices[], uint32_t Indices[], int ObjectId);
void gridHelper(int id, int xseg, int yseg, float gwidth, float gheight, glm::vec3 color, std::vector<Vertex> * verts, bool triangles=false);
/* width and height in vertices */
void generateLineIndices(int width, int height, std::vector<uint32_t> & indices);
void generateTriangleIndices(int width, int height, std::vector<uint32_t> & indices);

void createObjects(void);
void loadObject(Object * obj);
void calculateCamera(Viewport * vp);

glm::vec3 ClosestPoint(const glm::vec3 A, const glm::vec3 B, const glm::vec3 P, double *t);
void getNearAndFarClip(Viewport * viewport, glm::mat4 & model, glm::vec3 * near, glm::vec3 * far);

void drawScene(Viewport * viewport);
void clearObjects();
void cleanup(void);

static void keyboardCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow*, int, int, int);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

/* ###################### Globals and Constants ###################### */

#define PI 3.1415926535

// Define default width and height
static int g_FbWidth = 1066;
static int g_FbHeight = 600;
static float g_Time = 0.0f; // used for animation
static bool g_HasFocus = false;
static bool g_Debug = true;
static float flash = 0.0;

static float gCameraLatitude = 0;//PI/2;
static float gCameraLongitude = PI/16;
static float gCameraZoom = 15.0f;

Object * gridObj = NULL;

#define TITLE "Test Engine"

//////////////////////
// Objects
//////////////////////

// Object IDs
#define VAO_AXIS 0
#define VAO_GRID 1
#define VAO_CUBE 2

std::vector<Object *> gObjects;

const GLuint NumObjects = 3;	// number of different "objects" to be drawn
GLuint VertexArrayId[NumObjects] = { 0 };
GLuint VertexBufferId[NumObjects] = { 0 };
GLuint IndexBufferId[NumObjects] = { 0 };

uint32_t NumIndices[NumObjects] = { 0 };
size_t VertexBufferSize[NumObjects] = { 0 };
size_t IndexBufferSize[NumObjects] = { 0 };

// For dragging the camera
double gMouseDown[2] = {0.0};
static float gCameraLatitudeOnMouse;
static float gCameraLongitudeOnMouse;
static glm::vec3 gCameraCenterOnMouse;

////////////////////////////
// OpenGL Global Variables
////////////////////////////

GLFWwindow* window;
const GLuint window_width = g_FbWidth, window_height = g_FbHeight;

// Primary viewports
Viewport gViewport;

// Shaders
GLuint programID;
GLuint phongProgramID;
GLuint pickingProgramID;
GLuint hairProgramID;

// Shader uniforms
GLuint MatrixID, PhongMatrixID, ModID;
GLuint ModelMatrixID, PhongModelMatrixID;
GLuint ViewMatrixID, PhongViewMatrixID;
GLuint ProjMatrixID, PhongProjMatrixID;

GLuint HairViewMatrixID;
GLuint HairModelMatrixID;
GLuint HairProjMatrixID;

GLuint CameraID;
GLuint UseTexID, TextureID;

GLuint PickingMatrixID;
GLuint pickingColorID;

/* ###################### Function definitions ###################### */

void focusWindow(GLFWwindow* window, int focused) {
        g_HasFocus = focused;
}

void resizeWindow(GLFWwindow* window, int width, int height) {
	GLsizei w, h;

	glfwGetFramebufferSize(window, &w, &h);

	g_FbWidth = w;
	g_FbHeight = h;
	gViewport.projection = glm::perspective((float)PI/4, (float)g_FbWidth / g_FbHeight, 0.1f, 100.0f);
}

void calculateCamera(Viewport * vp)
{
	glm::mat4 m = glm::mat4(1.0);

	// we want to translate the camera out along an axis
	glm::quat q = glm::quat(glm::vec3(-gCameraLongitude, -gCameraLatitude, 0.0));
	glm::mat4 rot = mat4_cast(q);
	glm::mat4 trans = glm::translate(m, glm::vec3(0.0, 0.0, gCameraZoom));

	glm::vec4 pos = glm::translate(m, vp->center) * rot * trans * glm::vec4(glm::vec3(0.0), 1.0);

	vp->cameraPos.x = pos.x;
	vp->cameraPos.y = pos.y;
	vp->cameraPos.z = pos.z;

	vp->view = glm::lookAt(
			vp->cameraPos,
			vp->center,
			glm::vec3(0, 1.0, 0) // up vector
			);
}

void drawScene(Viewport * viewport)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	int vpW = (g_FbWidth - viewport->x*g_FbWidth)*viewport->width;
	int vpH = (g_FbHeight - viewport->y*g_FbHeight)*viewport->height;
	glScissor(viewport->x*g_FbWidth, viewport->y*g_FbHeight,
			vpW, vpH);
	glViewport(viewport->x*g_FbWidth, viewport->y*g_FbHeight,
			vpW, vpH);

	glClearColor(viewport->bgColor_RGB[0],
			viewport->bgColor_RGB[1],
			viewport->bgColor_RGB[2], 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(size_t i = 0; i < gObjects.size(); i++)
	{
		Object * o = gObjects[i];

		if(!o->visible)
			continue;

		glUseProgram(o->getShader());
		glEnable(GL_PROGRAM_POINT_SIZE);

		// no texture used
		if(o->texture != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, o->texture);
			glUniform1i(UseTexID, 1);
		}
		else
			glUniform1i(UseTexID, 0);

		//glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewport->view[0][0]);
		//glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &viewport->projection[0][0]);
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &o->getModelMatrix()[0][0]);
		glUniformMatrix4fv(PhongViewMatrixID, 1, GL_FALSE, &viewport->view[0][0]);
		glUniformMatrix4fv(PhongProjMatrixID, 1, GL_FALSE, &viewport->projection[0][0]);
		glUniformMatrix4fv(PhongModelMatrixID, 1, GL_FALSE, &o->getModelMatrix()[0][0]);
		glUniform1f(ModID, flash);
		//glUniform3f(CameraID, viewport->cameraPos.x, viewport->cameraPos.y, viewport->cameraPos.z);

		glBindVertexArray(VertexArrayId[o->id]);
		if(IndexBufferSize[o->id] > 0)
			glDrawElements(o->getType(), NumIndices[o->id], GL_UNSIGNED_INT, (void *)0);
		else
			glDrawArrays(o->getType(), 0, NumIndices[o->id]);

	}

	glUseProgram(0);
	glBindVertexArray(0);
}

glm::vec3 ClosestPoint(const glm::vec3 A, const glm::vec3 B,
		const glm::vec3 P, double *t)
{
	glm::dvec3 AB = glm::dvec3(B) - glm::dvec3(A);
	glm::dvec3 AP = glm::dvec3(P) - glm::dvec3(A);

	double ab_square = glm::dot(AB, AB); // |AB|^2
	double ap_dot_ab = glm::dot(AP, AB);
	// t is a projection param when we project vector AP onto AB
	*t = ap_dot_ab / ab_square;
	// calculate the closest point
	glm::vec3 Q = glm::dvec3(A) + AB * (*t);

	return Q;
}

void getNearAndFarClip(Viewport * viewport, glm::mat4 & model, glm::vec3 * near, glm::vec3 * far)
{
	double xpos, ypos;
	int width, height;

	// Get the cursor position in window coordinates
	glfwGetCursorPos(window, &xpos, &ypos);

	// Convert from window coordinates to framebuffer coordinates
	glfwGetWindowSize(window, &width, &height);

	float vpW = (width - viewport->x*width)*viewport->width;
	float vpH = (height - viewport->y*height)*viewport->height;

	// The most critical line here.
	ypos = vpH - ypos;

	glm::mat4 proj = viewport->projection;
	glm::mat4 modelview = viewport->view * model;
	glm::vec3 nearPlane = glm::vec3(xpos, ypos, 0.0);
	glm::vec3 farPlane = glm::vec3(xpos, ypos, 1.0);
	glm::vec4 viewportVec = glm::vec4(0.0, 0.0, vpW, vpH);

	*near = glm::unProject(nearPlane, modelview, proj, viewportVec);
	*far = glm::unProject(farPlane, modelview, proj, viewportVec);
}
static std::string getResource(const char * path) {
   return std::string("data/") + std::string(path);
}
static std::string getResource(std::string path) {
   return getResource(path.c_str());
}
static std::string getShader(const char * path) {
   return getResource(std::string("shader/") + std::string(path));
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, TITLE, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	// Regrab the framebuffer size as it may have changed out from under us
	glfwGetFramebufferSize(window, &g_FbWidth, &g_FbHeight);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, g_FbWidth / 2, g_FbHeight / 2);

	// Enable mouse & keyboard input
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Enable window callbacks for dynamic framebuffer sizing
	glfwSetWindowSizeCallback(window, resizeWindow);
	glfwSetFramebufferSizeCallback(window, resizeWindow);
	glfwSetWindowFocusCallback(window, focusWindow);

	return 0;
}

void initOpenGL(void)
{
	// Instantiate all viewports
	gViewport.name = "full";
	gViewport.x = 0;
	gViewport.y = 0;
	gViewport.width = 1.0;
	gViewport.height = 1.0;
	gViewport.bgColor_RGB[0] = 0.0;
	gViewport.bgColor_RGB[1] = 0.0;
	gViewport.bgColor_RGB[2] = 0.0;
	gViewport.cameraPos = glm::vec3(0.0, 1.00, 0.0); // eye
	gViewport.center = glm::vec3(0.0, -2.2, 0); // look at center of head
	gViewport.view = glm::lookAt(
			gViewport.cameraPos,
			gViewport.center,
			glm::vec3(0, 1, 0) // up vector
			);
	gViewport.projection = glm::perspective((float)PI/4, (float)g_FbWidth / g_FbHeight, 0.1f, 100.0f);

	calculateCamera(&gViewport);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	// Enable scissoring for viewport clearing
	glEnable(GL_SCISSOR_TEST);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders(getShader("StandardShading.vertexshader").c_str(), getShader("StandardShading.fragmentshader").c_str());
	phongProgramID = LoadShaders(getShader("PhongShading.vertexshader").c_str(), getShader("PhongShading.fragmentshader").c_str());
	pickingProgramID = LoadShaders(getShader("Picking.vertexshader").c_str(), getShader("Picking.fragmentshader").c_str());
	hairProgramID = LoadShaders(getShader("Hair.vertexshader").c_str(), getShader("Hair.fragmentshader").c_str(), getShader("Hair.geometryshader").c_str());

	if(programID == 0 || phongProgramID == 0 || pickingProgramID == 0 || hairProgramID == 0)
		exit(0);

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform

	PhongMatrixID = glGetUniformLocation(phongProgramID, "MVP");
	PhongViewMatrixID = glGetUniformLocation(phongProgramID, "V");
	PhongModelMatrixID = glGetUniformLocation(phongProgramID, "M");
	PhongProjMatrixID = glGetUniformLocation(phongProgramID, "P");
	CameraID = glGetUniformLocation(phongProgramID, "Camera");
	ModID = glGetUniformLocation(phongProgramID, "Mod");
	UseTexID = glGetUniformLocation(phongProgramID, "UseTex");
	TextureID = glGetUniformLocation(phongProgramID, "Texture");

	HairViewMatrixID = glGetUniformLocation(hairProgramID, "V");
	HairModelMatrixID = glGetUniformLocation(hairProgramID, "M");
	HairProjMatrixID = glGetUniformLocation(hairProgramID, "P");

	// Create dynamic object state for the first time
	createObjects();
}

void createVAOs(Vertex Vertices[], uint32_t Indices[], int ObjectId) {

	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;
	const size_t Textureoffset = sizeof(Vertices[0].Normal) + Normaloffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset); 
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Textureoffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal
	glEnableVertexAttribArray(3);	// tex

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(stderr, "ERROR: Could not create a VBO\n");
	}
}

void gridHelper(int id, int xseg, int yseg, float gwidth, float gheight, glm::vec3 color, std::vector<Vertex> * verts, bool triangles)
{
	/*
	 * Assume we are building a grid with its center at the origin.
	 * We are able to translate the grid to anywhere we please.
	 *
	 *     X
	 *   +---> w=2, l=2,
	 * Z |
	 *   |  TL
	 *   v  +----+----+        12 unique lines
	 *      |0   |1   |2       24 indices (for GL_LINE)
	 *      |    |    |
	 *      +----+----+        lines = 2*3 + 3*2
	 *      |3   |4   |5       lines = w*(l+1) + (w+1)*l
	 *      |    |    |
	 *      +----+----+  BR
	 *       6    7    8
	 */

	Vertex bottomRight = {{gwidth/2, 0.0, gheight/2, 1.0}, {color.r, color.g, color.b, 1.0}, {0.0, 1.0, 0.0, 0.0}};
	Vertex topLeft = {{-gwidth/2, 0.0, -gheight/2, 1.0}, {color.r, color.g, color.b, 1.0}, {0.0, 1.0, 0.0, 0.0}};

	// the number of squares along each axis
	int width = xseg, length = yseg;
	// the number of actual vertices per axis
	int widthV = width+1, lengthV = length+1;
	// the number of unique lines in the grid
	int numLines = width*lengthV + widthV*length;

	float dx = bottomRight.Position[0] - topLeft.Position[0];
	float dz = bottomRight.Position[2] - topLeft.Position[2];
	float strideX = dx/width;
	float strideZ = dz/length;

	Vertex * grid = new Vertex[widthV*lengthV];
	float v = 1.0;

	// Generate vertices
	for(int l = 0; l < lengthV; l++)
	{
		float Z = topLeft.Position[2] + strideZ*l;
		// XXX: U and V are messed up here because of the input image
		float u = 0.0;

		for(int w = 0; w < widthV; w++)
		{
			// initialize the vertex memory
			grid[l*widthV + w] = topLeft;

			grid[l*widthV + w].Position[0] = topLeft.Position[0] + strideX*w;
			//grid[l*widthV + w].Position[1] = (float)rand()/RAND_MAX*(1.0 + rand() % 2);
			grid[l*widthV + w].Position[2] = Z;

			// XXX: U and V are flipped because of the texture...
			grid[l*widthV + w].Texture[0] = v;
			grid[l*widthV + w].Texture[1] = u;

			u += 0.999/(widthV-1);
		}

		v -= 0.999/(lengthV-1);
	}

	// Export vertices if wanted
	if(verts)
		verts->assign(grid, grid +widthV*lengthV);

	// Emit drawing commands
	std::vector<uint32_t> indices;

	if(triangles) {
	        generateTriangleIndices(widthV, lengthV, indices);
	} else {
	        generateLineIndices(widthV, lengthV, indices);
	}

	NumIndices[id] = indices.size();
	IndexBufferSize[id] = sizeof(uint32_t)*indices.size();
	VertexBufferSize[id] = sizeof(Vertex)*widthV*lengthV;

	printf("Grid: %d verts, %lu indices\n", widthV*lengthV, indices.size());

	createVAOs(grid, &indices[0], id);

	delete [] grid;
}

void generateLineIndices(int width, int height, std::vector<uint32_t> & indices)
{
	indices.clear();

   for(int l = 0; l < height; l++)
   {
      int base = l*width;

      for(int w = 0; w < width; w++)
      {
         // horizontal
         if(w+1 != width)
         {
            indices.push_back(base + w);
            indices.push_back(base + w + 1);
         }

         // vertical
         if(l+1 != height)
         {
            indices.push_back(base + w);
            indices.push_back((l+1)*width + w);
         }
      }
   }
}

void generateTriangleIndices(int width, int height, std::vector<uint32_t> & indices)
{
	indices.clear();

   for(int s = 0; s < height-1; s++) {
      int base = s*width;

      for(int w = 0; w < width-1; w++) {
	 indices.push_back(base+width+w+1);
	 indices.push_back(base+w+1);
	 indices.push_back(base+w);

	 //printf("<%d, %d, %d>\n", indices[i], indices[i+1], indices[i+2]);
	 indices.push_back(base+w);
	 indices.push_back(base+width+w);
	 indices.push_back(base+width+w+1);
      }
   }
}

void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 5.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 5.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 5.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
	};

	Vertex CubeVerts[] = {
	   { {-1.0, -1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {1.0, -1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {1.0, -1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {-1.0, -1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },

	   { {-1.0, 1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {1.0, 1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {1.0, 1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	   { {-1.0, 1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0} },
	};

	VertexBufferSize[VAO_AXIS] = sizeof(CoordVerts);
	NumIndices[VAO_AXIS] = 6;
	IndexBufferSize[VAO_AXIS] = 0;

	createVAOs(CoordVerts, NULL, VAO_AXIS);
	/////////////////
	std::vector<uint32_t> indices;

	// Wireframe
	//for(int i = 0; i < 4; i++) {
	//   // point to point bottom
	//   indices.push_back(i);
	//   indices.push_back((i+1) % 4);

	//   // bottom to top line
	//   indices.push_back(i);
	//   indices.push_back(i+4);

	//   // point to point top
	//   indices.push_back(i+4);
	//   indices.push_back(4 + ((i+1) % 4));
	//}
	uint32_t CubeIndices[] = {
	   0, 1, 2,  2, 3, 0, // bottom
	   6, 5, 4,  4, 7, 6, // top

	   0, 4, 1,  4, 5, 1, // back
	   2, 6, 3,  6, 7, 3, // front

	   5, 2, 1,  5, 6, 2, // right
	   0, 3, 7,  0, 7, 4, // left
	};

	VertexBufferSize[VAO_CUBE] = sizeof(CubeVerts);
	NumIndices[VAO_CUBE] = sizeof(CubeIndices)/sizeof(uint32_t);
	IndexBufferSize[VAO_CUBE] = sizeof(CubeIndices);

	createVAOs(CubeVerts, CubeIndices, VAO_CUBE);

	Object * cube = new Object("", VAO_CUBE, GL_TRIANGLES, phongProgramID, NULL);
	cube->position = glm::vec3(0.0, 1.0, 0.0);
	gObjects.push_back(cube);

	//Object * axes = new Object("", VAO_AXIS, GL_LINES, programID, NULL);
	//gObjects.push_back(axes);

	//-- Hair --//

	//-- GRID --//

	// Grid base
	gridHelper(VAO_GRID, 200, 200, 100.0, 100.0, glm::vec3(0.0, 1.0, 1.0), NULL);
	gridObj = new Object("", VAO_GRID, GL_LINES, programID, NULL);
	gridObj->position = glm::vec3(0.0, 0.0, 0.0);

	gObjects.push_back(gridObj);

	////////////////
}

void loadObject(Object * obj)
{
	std::vector<glm::vec3> out_vertices;
	std::vector<glm::vec3> out_normals;
	GLuint id = obj->id;

	loadOBJ(obj->getPath(), out_vertices, out_normals);

	Vertex* Verts = new Vertex[out_vertices.size()];
	//GLushort* Idcs;

	for(size_t i = 0; i < out_vertices.size(); i++)
	{
		glm::vec3 v = out_vertices[i];
		glm::vec3 n = out_normals[i];

		Verts[i].Position[0] = v.x;
		Verts[i].Position[1] = v.y;
		Verts[i].Position[2] = v.z;
		Verts[i].Position[3] = 1.0;

		Verts[i].Normal[0] = n.x;
		Verts[i].Normal[1] = n.y;
		Verts[i].Normal[2] = n.z;
		Verts[i].Normal[3] = 0.0;
		// normals are vectors. they can't be translated

		Verts[i].Color[0] = obj->color.x;
		Verts[i].Color[1] = obj->color.y;
		Verts[i].Color[2] = obj->color.z;
		Verts[i].Color[3] = obj->color.w;
	}

	VertexBufferSize[id] = sizeof(Vertex)*out_vertices.size();
	NumIndices[id] = out_vertices.size();
	IndexBufferSize[id] = 0;

	gObjects.push_back(obj);
	createVAOs(Verts, NULL, id);

	delete [] Verts;
}

void clearObjects()
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);

		VertexBufferId[i] = VertexArrayId[i] = IndexBufferId[i] = NumIndices[i] = 0;
		VertexBufferSize[i] = IndexBufferSize[i] = 0;
	}

	for(size_t i = 0; i < gObjects.size(); i++) {
		delete gObjects[i];
	}

	gObjects.clear();
}

void cleanup(void)
{
	clearObjects();

	glDeleteProgram(programID);
	glDeleteProgram(phongProgramID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void keyboardCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key)
		{
			case GLFW_KEY_LEFT:
				gCameraLatitude += PI/40.0;
				calculateCamera(&gViewport);
				break;
			case GLFW_KEY_RIGHT:
				gCameraLatitude -= PI/40.0;
				calculateCamera(&gViewport);
				break;
			case GLFW_KEY_UP:
				gCameraLongitude += PI/40.0;
				calculateCamera(&gViewport);
				break;
			case GLFW_KEY_DOWN:
				gCameraLongitude -= PI/40.0;
				calculateCamera(&gViewport);
				break;
			case GLFW_KEY_R:
				clearObjects();
				createObjects();
				break;
			default:
				break;
		}
	}
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if(action == GLFW_PRESS)
		{
			int sizes[2];

			glfwGetCursorPos(window, &gMouseDown[0], &gMouseDown[1]);
			glfwGetWindowSize(window, &sizes[0], &sizes[1]);

			// Save our current camera position
			gCameraLatitudeOnMouse = gCameraLatitude;
			gCameraLongitudeOnMouse = gCameraLongitude;
			gCameraCenterOnMouse = gViewport.center;

			gMouseDown[0] /= sizes[0];
			gMouseDown[1] /= sizes[1];
		}
		else if(action == GLFW_RELEASE)
		{
		}
	}
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{

	float newZoom = gCameraZoom + yoffset;

	if(newZoom < 0.5f || newZoom >= 40.0f) {
		return;
	}

	gCameraZoom = newZoom;
	calculateCamera(&gViewport);
}

DWORD chan;

int start_sound()
{
	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		printf("An incorrect version of BASS was loaded\n");
		return 0;
	}

	// initialize BASS
	if (!BASS_Init(-1,44100,0,0,NULL)) {
		printf("Can't initialize sound device\n");
		return 0;
	}

	char * file_name = "./data/sound/FIVE_SUNS-lunar-orbit.ogg";

	if (!(chan=BASS_StreamCreateFile(FALSE,(void*)file_name,0,0,BASS_SAMPLE_LOOP))
		&& !(chan=BASS_MusicLoad(FALSE,(void*)file_name,0,0,BASS_MUSIC_RAMP|BASS_SAMPLE_LOOP,1))) {
		printf("Can't play file\n");
	} else {
		BASS_ChannelPlay(chan,FALSE);
		QWORD bytes = BASS_ChannelSeconds2Bytes(chan, 30.0);
		BASS_ChannelSetPosition(chan, bytes, BASS_POS_BYTE);
	}

	return 1;
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	start_sound();

	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	float hit = 0;
	double lastTime = glfwGetTime();
	double startTime = lastTime;
	int nbFrames = 0;
	do {

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;

		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("FPS %d\n", nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			double newPos[2];
			double delta[2];
			int sizes[2];

			glfwGetCursorPos(window, &newPos[0], &newPos[1]);
			glfwGetWindowSize(window, &sizes[0], &sizes[1]);

			newPos[0] /= sizes[0];
			newPos[1] /= sizes[1];

			delta[0] = newPos[0] - gMouseDown[0];
			delta[1] = newPos[1] - gMouseDown[1];

			//printf("%lf, %lf\n", delta[0], delta[1]);
			if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
					glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
			{
				glm::vec3 pointToCenter = gViewport.center - gViewport.cameraPos;

				glm::vec3 leftVec = glm::normalize(glm::vec3(-pointToCenter.z, 0.0, pointToCenter.x));
				glm::vec3 upVec = glm::normalize(glm::cross(leftVec, pointToCenter));

				// Scale the zoom factor with how close we are to the orbit point
				float scale = gCameraZoom;

				gViewport.center = gCameraCenterOnMouse - scale*leftVec*(float)delta[0] + scale*upVec*(float)delta[1];
			} else {
				gCameraLatitude = delta[0]*1.2 + gCameraLatitudeOnMouse;
				gCameraLongitude = delta[1]*1.2 + gCameraLongitudeOnMouse;
			}

			calculateCamera(&gViewport);
		}

		// Run animations
		g_Time = glfwGetTime() - startTime;

		gViewport.center = glm::vec3(0.0, -2.00, -g_Time*1.0+10.0); // eye


#define SPECHEIGHT 100
#define BANDS 28

		int bins[BANDS] = {0};
		float fft[1024]; // get the FFT data

		BASS_ChannelGetData(chan,fft,BASS_DATA_FFT2048);

		int b0 =0;
		int x,y = 0;

		// decay
		if (flash > 0.0)
		   flash -= 0.02;

		for (x=0;x<BANDS;x++) {
			float peak=0;
			int b1=pow(2,x*10.0/(BANDS-1));

			if (b1>1023) b1=1023;
			if (b1<=b0) b1=b0+1; // make sure it uses at least 1 FFT bin
			for (;b0<b1;b0++)
				if (peak<fft[1+b0]) peak=fft[1+b0];
			y=sqrt(peak)*3*SPECHEIGHT-4; // scale it (sqrt to make low values more visible)
			//if (y>SPECHEIGHT) y=SPECHEIGHT; // cap it
			bins[x] = y;
		}

		int min_hit = 70, max_hit = 100;
		if (bins[10] > min_hit) {
			if (bins[10] > max_hit)
			   bins[10] = max_hit;
			int range = max_hit - min_hit;
			float adj = ((float)(bins[10]-min_hit)/range);
			printf("%.2f\n", adj);
			flash = 1.0;
		}

		//gridObj->position = glm::vec3(0.0, 0.0, g_Time*1.0 - 30);
		calculateCamera(&gViewport);

		// Draw objects
		drawScene(&gViewport);


		// Swap buffers
		glfwSwapBuffers(window);

		// Prevent no vsync when minimized (C++11)
		if (!g_HasFocus) {
		   std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}
