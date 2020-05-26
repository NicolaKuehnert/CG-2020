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

float rotationstep = 30; //Size of a rotation-step in degrees, should be divisor of 360
float rotationx = 0.f;
float rotationy = 0.f;
float rotationz = 0.f;

float X_VIEW =0.0f, Y_VIEW=0.0f, Z_VIEW=4.0f;

float zNear = 0.1f;
float zFar  = 100.0f;
int n = 0;
float r = 1.0f;
int num = 0;

/*
Struct to hold data for object rendering.
*/
Object Kugel;

/*
* Calculate CMY from RGB
* returns Pointer to array of float[3] with CMY values
*/
float* calcRGBtoCMY(float r, float g, float b) {
    float c, m, y;
    c = 1 - r; // Cyan = 1 - red
    m = 1 - g; // Magenta = 1 - green
    y = 1 - b; // Yellow = 1- blue

    static float arr[] = { c, m, y };
    std::cout << "C: " << c << "; M: " << m << "; Y: " << y << std::endl;
    return arr;
}

/*
* Calculate RGB from CMY
* returns Pointer to array of float[3] with RGB values
*/
float* calcCMYtoRGB(float c, float m, float y) {
    float r, g, b;
    r = 1 - c;
    g = 1 - m;
    b = 1 - y;

    float arr[] = { r,g,b };
    std::cout << "R: " << r << "; G: " << g << "; B: " << b << std::endl;
    return arr;
}

/*
* Calculate HSV from RGB
* returns Pointer to array of float[3] with RGB values
*/
float* calcRGBtoHSV(float r, float g, float b) {
    float h, s, v;
    float max = 0, min = 1;
    if (max < r) max = r;
    if (max < g) max = g;
    if (max < b) max = b;

    if (min > r) min = r;
    if (min > g) min = g;
    if (min > b) min = b;

    //höchster Wert bestimmt Helligkeit
    v = max;

    //Wenn max == min und r==g==b
    if (max == min && r == b && r == g) h = 0;
    // max == r => h = 60 * (0 + ((g-b)/(max-min)))
    else if (max == r) {
        h = 60 * ((g - b) / (max - min));
    }
    // max == g => h = 60 * (2 + ((b-r)/(max-min)))
    else if (max == g) {
        h = 60 * (2 + ((b - r) / (max - min)));
    }
    // max == b => h = 60 * (4 + ((r-g)/(max-min)))
    else if (max == b) {
        h = 60 * (4 + ((r - g) / (max - min)));
    }

    if (h < 0) {
        h += 360;
    }

    //max == 0 => r==g==b==0 also kein Farbton
    if (max == 0) s = 0;
    else {
        s = (max - min) / max;
    }

    float arr[] = { h,s,v };

    std::cout << "H: " << h << "; S: " << s << "; V: " << v << std::endl;

    return arr;
}

/*
* Calculate RGB from HSV
* returns Pointer to array of float[3] with RGB values
*/
float* calcHSVtoRGB(float h, float s, float v) {
    int hi = h / 60;
    float f = (h / 60) - hi;

    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    float r, g, b;

    switch (hi)
    {
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
        // 0 oder 6
    default:
        r = v;
        g = t;
        b = p;
        break;
    }

    float arr[] = { r,g,b };
    std::cout << "R: " << r << "; G: " << g << "; B: " << b << std::endl;
    return arr;
}

void drawTriangle(float v1x, float v1y, float v1z,
    float v2x, float v2y, float v2z,
    float v3x, float v3y, float v3z) {

    Object triangle;
    triangle.vertices = { {v1x,v1y,v1z},{v2x,v2y,v2z},{v3x,v3y,v3z} };
    triangle.colors = { {1.0f,1.0f,0.0f},{1.0f,1.0f,0.0f},{1.0f,1.0f,0.0f} };
    triangle.indices = { 0,1,2 };
    GLuint programId = program.getHandle();
    

    triangle.init(programId, glm::vec3{ 0.0f,0.0f,0.0f }, rotationx, rotationy, rotationz);
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
void initTriangle()
{/*
    // Kanten, Farben, Indizes zuweisen
    Kugel.vertices = {
        { r, 0.0f, 0.0f }, //0 
        { 0.0f, r, 0.0f }, //1
        { 0.0f, 0.0f, r }, //2

        { -r, 0.0f, 0.0f }, //3
        { 0.0f, -r, 0.0f }, //4
        { 0.0f, 0.0f, -r }  //5
    };*/

    //              0               1               2
    subdivide(r, 0.0f, 0.0f, 0.0f,  r, 0.0f, 0.0f, 0.0f, r, n);
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
    subdivide(r, 0.0f, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    //              4               3               5
    subdivide(0.0f, -r, 0.0f, -r, 0.0f, 0.0f, 0.0f, 0.0f, -r, n);
    
}

/*void renderTriangle(Object triangle) {
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Kugel.render(GL_TRIANGLES, 3, view, projection, program); 
}
*/
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
  initTriangle();
  
  return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	initTriangle();
	//renderQuad();
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
  projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
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
            initTriangle();
        }
        break;
    case 'r':
        if (r > 1) {
            r--;
            initTriangle();
        }
        break;
    case '+':
        if (n < 4) {
            n++;
            initTriangle();
        }
        break;
    case '-':
        if (n > 0) {
            n--;
            initTriangle();
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
  //glutIdleFunc   (glutDisplay); // redisplay when idle
  
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

