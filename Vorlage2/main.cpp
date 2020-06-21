#include <iostream>
#include <vector>
#include <sstream>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

#include "AbstractCube.h"
#include "CubeSharp.h"
#include "CubeSmooth.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800
// GLUT window id/handle
int glutID = 0;
bool wire = true;

cg::AbstractCube* sun   = nullptr;
cg::AbstractCube* planet  = nullptr;
cg::AbstractCube* moon1 = nullptr;
cg::AbstractCube* moon2 = nullptr;

cg::AbstractCube* manipulate = nullptr;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar = 100.0f;

/*
Release resources on termination.
*/
void release()
{
	delete planet;
	delete sun;
	delete moon1;
	delete moon2;
}

unsigned  lightIndex = 0;
glm::vec4 lights[2] = {
	{ 0.0f, -1.0f, 0.0f, 0.0f },
	{ 0.0f,  0.0f, 0.0f, 1.0f }
};
/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL stuff. Set "background" color and enable depth testing.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable    (GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Construct view matrix.
	glm::vec3 eye(0.0f, 0.0f, 40.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(eye, center, up);

	// Create cube objects.
	try
	{
		sun   = new cg::CubeSharp(5.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		planet  = new cg::CubeSharp(2.0f, glm::vec3(0.f, 1.0f, 0.0f));
		moon1 = new cg::CubeSharp(1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		moon2 = new cg::CubeSharp(1.0f, glm::vec3(0.0f, 0.0f, 1.0f));


		manipulate = sun;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	// Manipulate models.
	auto& modelRight = planet->getModel();
	modelRight = glm::translate(modelRight, glm::vec3(15.0f, 0.0f, 0.0f));
	auto& modelMoon1 = moon1->getModel();
	modelMoon1 = glm::translate(modelMoon1, glm::vec3(15.0f, 5.0f, 0.0f));
	auto& modelMoon2 = moon2->getModel();
	modelMoon2 = glm::translate(modelMoon2, glm::vec3(15.0f, -5.0f, 0.0f));

	// Set light (only an example).
	sun ->setLightVector(lights[lightIndex]);
	planet->setLightVector(lights[lightIndex]);
	moon1->setLightVector(lights[lightIndex]);
	moon2->setLightVector(lights[lightIndex]);

	std::cout << "Use space to switch between cubes" << std::endl;
	std::cout << "Use x, y, z to rotate the currently selected cube" << std::endl;
	std::cout << "Use + and - to scale the currently selected cube" << std::endl;
	std::cout << "Use n to enable / disable normals for the currently selected cube" << std::endl;

	return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sun ->render(view, projection);
	planet->render(view, projection);
	moon1->render(view, projection);
	moon2->render(view, projection);
}
/*
 Display callback.
 */
void glutDisplay ()
{
   render();
   glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
	height = height < 1 ? 1 : height;
	glViewport(0, 0, width, height);

	// Construct projection matrix.
	projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	auto& model = manipulate->getModel();

	switch (keycode)
	{
	case '+':
		model = glm::scale(model, glm::vec3(1.2f));
		break;
	case '-':
		model = glm::scale(model, glm::vec3(0.8f));
		break;
	case 'x':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		break;
	case 'X':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		break;
	case 'y':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		break;
	case 'Y':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		break;
	case 'z':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		break;
	case 'Z':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		break;
	case ' ':
		manipulate = manipulate == sun ? planet : sun;

		break;
	case '1': {
		lightIndex = 1 - lightIndex;
		sun ->setLightVector(lights[lightIndex]);
		planet->setLightVector(lights[lightIndex]);
		std::ostringstream os; os << "05 - LightVector " << glm::to_string(lights[lightIndex]) << std::flush;
		glutSetWindowTitle(os.str().c_str());
		break;
	}
	case 'n':
		manipulate->toggleRenderNormals();
		break;
	case 'w':
		wire = wire == true ? false : true;
		break;
	}
	
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);

    // GLUT: Create a window and opengl context (version 4.3 core profile).
    glutInitContextVersion(4, 3);
    glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("05 - LightVector vec4(0.0, -1.0, 0.0, 0.0)");
    glutID = glutGetWindow();
	  
    // GLEW: Load opengl extensions
    glewExperimental = GL_TRUE;
    GLenum result = glewInit();

    if (result != GLEW_OK) {
       return -1;
    }

    // GLUT: Set callbacks for events.
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    //glutIdleFunc   (glutDisplay); // redisplay when idle

    glutKeyboardFunc(glutKeyboard);

    // init vertex-array-objects.
    {
       bool result = init();
       if (!result) {
	 release();
	 return -2;
       }
    }

    // GLUT: Loop until the user closes the window
    // rendering & event handling
    glutMainLoop ();

    // Cleanup everything on termination.
    release();
	
    return 0;
}
