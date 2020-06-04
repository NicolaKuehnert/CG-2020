#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"
#include "Object.h"

// Standard window width
const int WINDOW_WIDTH  = 1200;
// Standard window height
const int WINDOW_HEIGHT = 1000;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

//Size of a rotation-step in degrees, should be divisor of 360
float rotationstep = 1; 

float rotationx = 0.f;
float rotationy = 0.f;
float rotationz = 0.f;

//Relationship of the length of the axes drawn inside the sphere -- 1 means drawn axes match radius of the sphere
float faxislength = 1.5f;

float X_VIEW =0.0f, Y_VIEW=0.0f, Z_VIEW=50.0f;

float zNear = 0.1f;
float zFar  = 100.0f;
int n = 2;
float r = 3.0f;
int num = 0;

glm::vec3 red = { 1.0f,0.0f,0.0f };
glm::vec3 green = { 0.0f,1.0f,0.0f };
glm::vec3 blue = { 0.0f,0.0f,1.0f };
glm::vec3 yellow = { 1.0f,1.0f,0.0f };

/*
Struct to hold data for object rendering.
*/
Object Kugel;
Object Planet;
Object Mond1;
Object Mond2;

void drawTriangle(float v1x, float v1y, float v1z,
    float v2x, float v2y, float v2z,
    float v3x, float v3y, float v3z) {

    Object triangle;
    triangle.vertices = { {v1x,v1y,v1z},{v2x,v2y,v2z},{v3x,v3y,v3z} };
    triangle.colors = { {0.0f,1.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,1.0f,0.0f} };
    triangle.indices = { 0,1,2 };
    GLuint programId = program.getHandle();
    

    triangle.init(programId, glm::vec3{ 0.0f,0.0f,0.0f });
    
    triangle.model = glm::rotate(triangle.model, glm::radians(rotationx), glm::vec3(1, 0, 0));
    triangle.model = glm::rotate(triangle.model, glm::radians(rotationy), glm::vec3(0, 1, 0));
    triangle.model = glm::rotate(triangle.model, glm::radians(rotationz), glm::vec3(0, 0, 1));
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    triangle.render(GL_TRIANGLES, 3, view, projection, program);

}

void subdivide(float v1x, float v1y, float v1z,
                float v2x, float v2y, float v2z,
                float v3x, float v3y, float v3z,
                int level) {
    if (level == 0) {
        // Reached desired tessellation level, emit triangle.
        drawTriangle(v1x, v1y, v1z,
            v2x, v2y, v2z,
            v3x, v3y, v3z);
    }
    else {
        // Calculate middle of first edge...
        float v12x = (r / 2) * (v1x + v2x);
        float v12y = (r / 2) * (v1y + v2y);
        float v12z = (r / 2) * (v1z + v2z);
        // ... and renormalize it to get a point on the sphere.
        float s = r / sqrt(v12x * v12x + v12y * v12y + v12z * v12z);
        v12x *= s;
        v12y *= s;
        v12z *= s;

        // Same thing for the middle of the other two edges.
        float v13x = (r / 2) * (v1x + v3x);
        float v13y = (r / 2) * (v1y + v3y);
        float v13z = (r / 2) * (v1z + v3z);
        s = r / sqrt(v13x * v13x + v13y * v13y + v13z * v13z);
        v13x *= s;
        v13y *= s;
        v13z *= s;

        float v23x = (r / 2) * (v2x + v3x);
        float v23y = (r / 2) * (v2y + v3y);
        float v23z = (r / 2) * (v2z + v3z);
        s = r / sqrt(v23x * v23x + v23y * v23y + v23z * v23z);
        v23x *= s;
        v23y *= s;
        v23z *= s;

        // Make the recursive calls.
        subdivide(v1x, v1y, v1z,
            v12x, v12y, v12z,
            v13x, v13y, v13z,
            level - 1);
        subdivide(v12x, v12y, v12z,
            v2x, v2y, v2z,
            v23x, v23y, v23z,
            level - 1);
        subdivide(v13x, v13y, v13z,
            v23x, v23y, v23z,
            v3x, v3y, v3z,
            level - 1);
        subdivide(v12x, v12y, v12z,
            v23x, v23y, v23z,
            v13x, v13y, v13z,
            level - 1);
    }
}



/*
* Initialisiert alle Dreiecke
*/
void initSun()
{
    Kugel.vertices = {
        { r, 0.0f, 0.0f }, //0 
        { 0.0f, r, 0.0f }, //1
        { 0.0f, 0.0f, r }, //2

        { -r, 0.0f, 0.0f }, //3
        { 0.0f, -r, 0.0f }, //4
        { 0.0f, 0.0f, -r }  //5
    };
    Kugel.colors = {
        yellow, //0 
        yellow, //1
        yellow, //2

        yellow, //3
        yellow, //4
        yellow  //5
    };
    Kugel.indices = {
        0,1,2,
        0,4,2,
        1,2,3,
        4,2,3,
        0,1,5,
        0,4,5,
        1,3,5,
        4,3,5
    };
    GLuint programId = program.getHandle();
    Kugel.init(programId, { 0.0f,0.0f,0.0f });
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Kugel.render(GL_TRIANGLES, 24, view, projection, program);
    /*
    //              0               1               2
    subdivide(r,0.0f,0.0f,  0.0f,r,0.0f,  0.0f,0.0f,r,   n);
    //              0               4               2
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, r, n);
    //              1               2               3
    subdivide(0.0f, r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              4               2               3
    subdivide(0.0f, -r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              0               1               5
    subdivide(r, 0.0f, 0.0f, 0.0f, r, 0.0f, 0.0f, 0.0f, -r, n);
    //              0               4               5
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, -r, n);
    //              1               3               5
    subdivide(0.0f, r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    //              4               3               5
    subdivide(0.0f, -r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    */

}

void initPlanet()
{
    Planet.vertices = {
        { 2.0f, 0.0f, 0.0f }, //0 
        { 0.0f, 2.0f, 0.0f }, //1
        { 0.0f, 0.0f, 2.0f }, //2

        { -2.0f, 0.0f, 0.0f }, //3
        { 0.0f, -2.0f, 0.0f }, //4
        { 0.0f, 0.0f, -2.0f }  //5
    };
    Planet.colors = {
        green, //0 
        green, //1
        green, //2

        green, //3
        green, //4
        green  //5
    };
    Planet.indices = {
        0,1,2,
        0,4,2,
        1,2,3,
        4,2,3,
        0,1,5,
        0,4,5,
        1,3,5,
        4,3,5
    };
    GLuint programId = program.getHandle();
    Planet.init(programId, { 15.0f,0.0f,0.0f });
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Planet.model = glm::translate(Planet.model, { -15.0f,0.0f,0.0f });
    Planet.model = glm::rotate(Planet.model, glm::radians(rotationy), glm::vec3(0, 1, 0));
    Planet.model = glm::translate(Planet.model, { 15.0f,0.0f,0.0f });
    Planet.model = glm::rotate(Planet.model, glm::radians(rotationy), glm::vec3(0, 1, 0));
    Planet.render(GL_TRIANGLES, 24, view, projection, program);
    /*
    //              0               1               2
    subdivide(r,0.0f,0.0f,  0.0f,r,0.0f,  0.0f,0.0f,r,   n);
    //              0               4               2
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, r, n);
    //              1               2               3
    subdivide(0.0f, r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              4               2               3
    subdivide(0.0f, -r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              0               1               5
    subdivide(r, 0.0f, 0.0f, 0.0f, r, 0.0f, 0.0f, 0.0f, -r, n);
    //              0               4               5
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, -r, n);
    //              1               3               5
    subdivide(0.0f, r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    //              4               3               5
    subdivide(0.0f, -r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    */

}

void initMond1()
{
    Mond1.vertices = {
        { 1.0f, 0.0f, 0.0f }, //0 
        { 0.0f, 1.0f, 0.0f }, //1
        { 0.0f, 0.0f, 1.0f }, //2

        { -1.0f, 0.0f, 0.0f }, //3
        { 0.0f, -1.0f, 0.0f }, //4
        { 0.0f, 0.0f, -1.0f }  //5
    };
    Mond1.colors = {
        blue, //0 
        blue, //1
        blue, //2

        blue, //3
        blue, //4
        blue  //5
    };
    Mond1.indices = {
        0,1,2,
        0,4,2,
        1,2,3,
        4,2,3,
        0,1,5,
        0,4,5,
        1,3,5,
        4,3,5
    };
    GLuint programId = program.getHandle();
    Mond1.init(programId, { 15.0f,5.0f,0.0f });
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Mond1.model = glm::translate(Mond1.model, { -15.0f,-5.0f,0.0f });
    Mond1.model = glm::rotate(Mond1.model, glm::radians(rotationy), glm::vec3(0, 1, 0));
    Mond1.model = glm::translate(Mond1.model, { 15.0f,5.0f,0.0f });

    Mond1.model = glm::translate(Mond1.model, { -15.0f,-5.0f,0.0f });
    Mond1.model = glm::rotate(Mond1.model, glm::radians(rotationx), glm::vec3(1, 0, 0));
    Mond1.model = glm::translate(Mond1.model, { 15.0f,5.0f,0.0f });
    Mond1.render(GL_TRIANGLES, 24, view, projection, program);
    /*
    //              0               1               2
    subdivide(r,0.0f,0.0f,  0.0f,r,0.0f,  0.0f,0.0f,r,   n);
    //              0               4               2
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, r, n);
    //              1               2               3
    subdivide(0.0f, r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              4               2               3
    subdivide(0.0f, -r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              0               1               5
    subdivide(r, 0.0f, 0.0f, 0.0f, r, 0.0f, 0.0f, 0.0f, -r, n);
    //              0               4               5
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, -r, n);
    //              1               3               5
    subdivide(0.0f, r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    //              4               3               5
    subdivide(0.0f, -r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    */

}

void initMond2()
{
    Mond2.vertices = {
        { 1.0f, 0.0f, 0.0f }, //0 
        { 0.0f, 1.0f, 0.0f }, //1
        { 0.0f, 0.0f, 1.0f }, //2

        { -1.0f, 0.0f, 0.0f }, //3
        { 0.0f, -1.0f, 0.0f }, //4
        { 0.0f, 0.0f, -1.0f }  //5
    };
    Mond2.colors = {
        blue, //0 
        blue, //1
        blue, //2

        blue, //3
        blue, //4
        blue  //5
    };
    Mond2.indices = {
        0,1,2,
        0,4,2,
        1,2,3,
        4,2,3,
        0,1,5,
        0,4,5,
        1,3,5,
        4,3,5
    };
    GLuint programId = program.getHandle();
    Mond2.init(programId, { 15.0f,-5.0f,0.0f });
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Mond2.model = glm::translate(Mond2.model, { -15.0f,5.0f,0.0f });
    Mond2.model = glm::rotate(Mond2.model, glm::radians(rotationy), glm::vec3(0, 1, 0));
    Mond2.model = glm::translate(Mond2.model, { 15.0f,-5.0f,0.0f });

    Mond2.model = glm::translate(Mond2.model, { -15.0f,5.0f,0.0f });
    Mond2.model = glm::rotate(Mond2.model, glm::radians(rotationx), glm::vec3(1, 0, 0));
    Mond2.model = glm::translate(Mond2.model, { 15.0f,-5.0f,0.0f });
    Mond2.render(GL_TRIANGLES, 24, view, projection, program);

    /*
    //              0               1               2
    subdivide(r,0.0f,0.0f,  0.0f,r,0.0f,  0.0f,0.0f,r,   n);
    //              0               4               2
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, r, n);
    //              1               2               3
    subdivide(0.0f, r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              4               2               3
    subdivide(0.0f, -r, 0.0f, 0.0f, 0.0f, r, -r, 0.0f, 0.0f, n);
    //              0               1               5
    subdivide(r, 0.0f, 0.0f, 0.0f, r, 0.0f, 0.0f, 0.0f, -r, n);
    //              0               4               5
    subdivide(r, 0.0f, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, -r, n);
    //              1               3               5
    subdivide(0.0f, r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    //              4               3               5
    subdivide(0.0f, -r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    */

}

void initAxes() {
    Object xaxis, yaxis, zaxis;
    GLuint programId = program.getHandle();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //X-Achse
    xaxis.vertices = { {faxislength*r, 0.0f, 0.0f}, {faxislength*-r, 0.0f, 0.0f} };
    xaxis.colors = { {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    xaxis.indices = { 0,1 };
    xaxis.init(programId, glm::vec3{ 0.0f, 0.0f, 0.0f });
    //xaxis.render(GL_LINES, 2, view, projection, program);

    //Y-Achse
    yaxis.vertices = { {0.0f, faxislength*r, 0.0f}, {0.0f, faxislength*-r, 0.0f} };
    yaxis.colors = { red, red };
    yaxis.indices = { 0,1 };
    yaxis.init(programId, glm::vec3{ 0.0f, 0.0f, 0.0f });
    yaxis.render(GL_LINES, 2, view, projection, program);

    //Z-Achse
    zaxis.vertices = { {0.0f, 0.0f, faxislength*r}, {0.0f, 0.0f, faxislength*-r} };
    zaxis.colors = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };
    zaxis.indices = { 0,1 };
    zaxis.init(programId, glm::vec3{ 0.0f, 0.0f, 0.0f });
    //zaxis.render(GL_LINES, 2, view, projection, program);
}

void initView() {
    // Construct view matrix.
    // Camera
    glm::vec3 eye(X_VIEW, Y_VIEW, Z_VIEW);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(eye, center, up);
}
/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
  // OpenGL: Set "background" color and enable depth testing.
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  
  initView();
  
  // Create a shader program and set light direction.
  if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
    std::cerr << program.log();
    return false;
  }
  
  if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
    std::cerr << program.log();
    return false;
  }
  
  if (!program.link()) {
    std::cerr << program.log();
    return false;
  }

  //Init Objects here
  initSun();
  initPlanet();
  initMond1();
  initMond2();
  initAxes();
  
  return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	initSun();
    initPlanet();
    initMond1();
    initMond2();
    initAxes();
    
    rotationx += rotationstep;
    if (rotationx == 360) {
        rotationx = 0;
    }
    rotationy += rotationstep;
    if (rotationy == 360) {
        rotationy = 0;
    }
    rotationz += rotationstep;
    if (rotationz == 360) {
        rotationz = 0;
    }
}

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
  // Division by zero is bad...
  height = height < 1 ? 1 : height;
  glViewport(0, 0, width, height);
  
  // Construct projection matrix.
  //projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
  projection = glm::perspective(45.0f, (float)width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        return;
    case 'R':
        if (r < (Z_VIEW/2)) {
            r++;
            initSun();
        }
        break;
    case 'r':
        if (r > 1) {
            r--;
            initSun();
        }
        break;
    case '+':
        if (n < 4) {
            n++;
            initSun();
        }
        break;
    case '-':
        if (n > 0) {
            n--;
            initSun();
        }
        break;
    case 'a':
        Z_VIEW--;
        initView();
        break;
    case 's':
        Z_VIEW++;
        initView();
        break;
    case 'x':
        rotationx +=  rotationstep;
        if (rotationx == 360) {
            rotationx = 0;
        }
        break;
    case 'y':
        rotationy += rotationstep;
        if (rotationy == 360) {
            rotationy = 0;
        }
        break;
    case 'z':
        rotationz += rotationstep;
        if (rotationz == 360) {
            rotationz = 0;
        }
        break;
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{

  // GLUT: Initialize freeglut library (window toolkit).
  // Init Window
  glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(40,40);
  glutInit(&argc, argv);
  
  // GLUT: Create a window and opengl context (version 4.3 core profile).
  glutInitContextVersion(4, 3);
  glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  
  //Set window name
  glutCreateWindow("Aufgabenblatt 01");
  glutID = glutGetWindow();
  
  // GLEW: Load opengl extensions
  //glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    return -1;
  }
#if _DEBUG
  if (glDebugMessageCallback) {
    std::cout << "Register OpenGL debug callback " << std::endl;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
			  GL_DONT_CARE,
			  GL_DONT_CARE,
			  0,
			  nullptr,
			  true); // get all debug messages
  } else {
    std::cout << "glDebugMessageCallback not available" << std::endl;
  }
#endif

  // GLUT: Set callbacks for events.
  glutReshapeFunc(glutResize);
  glutDisplayFunc(glutDisplay);
  glutIdleFunc   (glutDisplay); // redisplay when idle
  
  glutKeyboardFunc(glutKeyboard);
  
  init();

  // GLUT: Loop until the user closes the window
  // rendering & event handling
  glutMainLoop ();
  
  // Cleanup in destructors:
  // Objects will be released in ~Object
  // Shader program will be released in ~GLSLProgram

  return 0;
}

