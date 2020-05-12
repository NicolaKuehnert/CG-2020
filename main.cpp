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

float zNear = 0.1f;
float zFar  = 100.0f;

/*
Struct to hold data for object rendering.
*/


Object triangle;
Object quad;
Object quad2;
Object quad3;

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


/*
* Liste alle Dreiecke, die gerendert werden
*/
void renderTriangle()
{
    triangle.render(GL_TRIANGLES, 3, view, projection, program);
   
}

/*
* Liste alle Quadrate, die gerendert werden
*/
void renderQuad()
{
    quad.render(GL_TRIANGLES, 6, view, projection, program);
    quad2.render(GL_TRIANGLES, 6, view, projection, program);
    quad3.render(GL_TRIANGLES, 6, view, projection, program);
    
    
}

/*
* Initialisiert alle Dreiecke
*/
void initTriangle()
{
    // Kanten, Farben, Indizes zuweisen
    triangle.vertices = { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
    triangle.colors = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
    triangle.indices = { 0, 1, 2 };

    GLuint programId = program.getHandle();

    // Dreiecke initialisieren
    triangle.init(programId, glm::vec3(-1.25f, 0.0f, 0.0f));
    
}

/*
* Initialisiert alle Quadrate
*/
void initQuad(float colors[3][3])
{
    // Kanten, Farben, Indizes zuweisen
    quad.vertices = { { -0.5f, 0.5f, 0.0f }, { -0.5, -0.5, 0.0 }, { 0.5f, -0.5f, 0.0f }, { 0.5f, 0.5f, 0.0f } };
    quad.colors = { { colors[0][0], colors[0][1], colors[0][2] }, { colors[0][0], colors[0][1], colors[0][2] }, { colors[0][0], colors[0][1], colors[0][2] }, { colors[0][0], colors[0][1], colors[0][2] }};
    quad.indices = { 0, 1, 2, 0, 2, 3 };

    quad2.vertices = { { 0.0f, 1.0f, 0.0f }, { 0.0, 0.0, 0.0 }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } };
    quad2.colors = { { colors[1][0], colors[1][1], colors[1][2] }, { colors[1][0], colors[1][1], colors[1][2] }, { colors[1][0], colors[1][1], colors[1][2] }, { colors[1][0], colors[1][1], colors[1][2] } };
    quad2.indices = { 0, 1, 2, 0, 2, 3 };

    quad3.vertices = { { -1.0f, 0.0f, 0.0f }, { -1.0, -1.0, 0.0 }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    quad3.colors = { { colors[2][0], colors[2][1], colors[2][2] }, { colors[2][0], colors[2][1], colors[2][2] }, { colors[2][0], colors[2][1], colors[2][2] }, { colors[2][0], colors[2][1], colors[2][2] } };
    quad3.indices = { 0, 1, 2, 0, 2, 3 };

    GLuint programId = program.getHandle();

    // Quadrate initialisieren
    //quad.init(programId, glm::vec3(1.25f, 1.0f, 0.0f));
    quad.init(programId, glm::vec3(0.75f, -0.6f, 0.0f));
    quad2.init(programId, glm::vec3(1.25f, -1.10f, 0.0f));
    quad3.init(programId, glm::vec3(1.25f, -1.10f, 0.0f));

}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init(float colors[3][3])
{
  // OpenGL: Set "background" color and enable depth testing.
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  
  // Construct view matrix.
  // Camera
  glm::vec3 eye(0.0f, 0.0f, 4.0f);
  glm::vec3 center(0.0f, 0.0f, 0.0f);
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  
  view = glm::lookAt(eye, center, up);
  
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

  // Create all objects.
  //initTriangle();
  initQuad(colors);
  
  return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//renderTriangle();
	renderQuad();
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
    glutDestroyWindow ( glutID );
    return;
    
  case '+':
    // do something
    break;
  case '-':
    // do something
    break;
  case 'x':
    // do something
    break;
  case 'y':
    // do something
    break;
  case 'z':
    // do something
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
    
    float r, g, b;
    
    float h, s, v;

    std::cout << "Ein Wert fuer Rot eingeben" << std::endl;;
    std::cin >> r;
    std::cout << "Ein Wert fuer Gruen eingeben" << std::endl;
    std::cin >> g;
    std::cout << "Ein Wert fuer Blau eingeben" << std::endl;
    std::cin >> b;

    calcRGBtoCMY(r, g, b);
    calcRGBtoHSV(r, g, b);

    std::cout << "Ein Wert fuer Hue eingeben" << std::endl;
    std::cin >> h;
    std::cout << "Ein Wert fuer Saturation eingeben" << std::endl;
    std::cin >> s;
    std::cout << "Ein Wert fuer Value eingeben" << std::endl;
    std::cin >> v;

    float* p = calcHSVtoRGB(h, s, v);
    r = *p;
    g = *(p + 1);
    b = *(p + 2);
    calcRGBtoCMY(r, g, b);
    
    float r2, g2, b2;
    float c, m, y;
    float h2, s2, v2;

    std::cout << "Faerben eines Quadrats nach RGB-Modell" << std::endl;
    std::cout << "Ein Wert fuer Rot eingeben" << std::endl;
    std::cin >> r2;
    std::cout << "Ein Wert fuer Gruen eingeben" << std::endl;
    std::cin >> g2;
    std::cout << "Ein Wert fuer Blau eingeben" << std::endl;
    std::cin >> b2;

    float r3, g3, b3;

    std::cout << "Faerben eines Quadrats nach CMY-Modell" << std::endl;
    std::cout << "Ein Wert fuer Cyan eingeben" << std::endl;
    std::cin >> c;
    std::cout << "Ein Wert fuer Magenta eingeben" << std::endl;
    std::cin >> m;
    std::cout << "Ein Wert fuer Yellow eingeben" << std::endl;
    std::cin >> y;

    p = calcCMYtoRGB(c, m, y);
    r3 = *p;
    g3 = *(p + 1);
    b3 = *(p + 2);

    float r4, g4, b4;

    std::cout << "Faerben eines Quadrats nach HSV-Modell" << std::endl;
    std::cout << "Ein Wert fuer Hue eingeben" << std::endl;
    std::cin >> h2;
    std::cout << "Ein Wert fuer Saturation eingeben" << std::endl;
    std::cin >> s2;
    std::cout << "Ein Wert fuer Value eingeben" << std::endl;
    std::cin >> v2;

    p = calcHSVtoRGB(h2, s2, v2);
    r4 = *p;
    g4 = *(p + 1);
    b4 = *(p + 2);
    
    float arr[3][3] = {{r2,g2,b2}, {r3,g3,b3}, {r4,g4,b4}};

    


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
  
  // init vertex-array-objects.
  bool result = init(arr);
  if (!result) {
    return -2;
  }

  // GLUT: Loop until the user closes the window
  // rendering & event handling
  glutMainLoop ();
  
  // Cleanup in destructors:
  // Objects will be released in ~Object
  // Shader program will be released in ~GLSLProgram

  return 0;
}

