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

float rotationx = 1.f;
float rotationy = 1.f;
float rotationz = 0.f;
float pos_y = 0.0f;

float X_VIEW =0.0f, Y_VIEW=0.0f, Z_VIEW=50.0f;

float zNear = 0.1f;
float zFar  = 100.0f;
int n = 4;
float r = 5.0f;
bool g = false;

// comment
glm::vec3 red = { 1.0f,0.0f,0.0f };
glm::vec3 green = { 0.0f,1.0f,0.0f };
glm::vec3 blue = { 0.0f,0.0f,1.0f };
glm::vec3 yellow = { 1.0f,1.0f,0.0f };

glm::vec3 MS = { 0.0f,0.0f,0.0f };
glm::vec3 MP = { 15.0f,0.0f,0.0f };

/*
Struct to hold data for object rendering.
*/
Object Kugel;
Object Planet;
Object Mond1;
Object Mond2;
Object xaxis, yaxis;

/*
* Initialisiert alle Objekte
* Ohne Render Funktion würden die Objekte jedes Mal neu initialisiert -> Speicherüberlauf!
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
    Kugel.init(programId, MS);

}

void renderSun() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Kugel.render(GL_TRIANGLES, 24, view, projection, program);
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
    Planet.init(programId, MP);
    

}

void renderPlanet() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Planet.render(GL_TRIANGLES, 24, view, projection, program);
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
    

}

void renderMond1() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    Mond1.render(GL_TRIANGLES, 24, view, projection, program);
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
    

}

void renderMond2(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    Mond2.render(GL_TRIANGLES, 24, view, projection, program);
}

void initAxes() {
    
    GLuint programId = program.getHandle();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Planet Axis
    xaxis.vertices = { {0.0f,3.0f,0.0f}, {0.0f,-3.0f,0.0f} };
    xaxis.colors = { red, red };
    xaxis.indices = { 0,1 };
    xaxis.init(programId, { 15.0f,0.0f,0.0f });

    

    //Sun Axis
    yaxis.vertices = { {0.0f, r+1, 0.0f}, {0.0f, -(r+1), 0.0f} };
    yaxis.colors = { red, red };
    yaxis.indices = { 0,1 };
    yaxis.init(programId, glm::vec3{ 0.0f, 0.0f, 0.0f });
    
    
}

void renderAxis() {

    xaxis.render(GL_LINES, 2, view, projection, program);
    yaxis.render(GL_LINES, 2, view, projection, program);
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
	renderSun();
    renderPlanet();
    renderMond1();
    renderMond2();
    renderAxis();

    //Ohne delay rendert das Programm viel zu schnell
    //d und f sollen schneller/langsamer machen mit flüssiger Animation
    //Erhöht man Rotationsweite -> Springen, also Animationsgeschwindigkeit (Rendergeschwindigkeit) verändern
    switch (n)
    {
    case 1:
        Sleep(30);
        break;
    case 2:
        Sleep(20);
        break;
    case 3:
        Sleep(10);
        break;
    case 4:
        Sleep(3);
        break;
    }

    /*
    Aufgabe 4
    Tastengruck g soll Rotation stoppen, außer Monde um Planeten

    TO-DO:
    Die beiden Rotationen passen noch nicht im Zusammenspiel.
    Funktionieren einzeln, aber nicht gemeinsam
    */
    if (g) {

        Planet.model = glm::translate(Planet.model, { -15.0f,0.0f,0.0f });
        Planet.model = glm::rotate(Planet.model, glm::radians(rotationy), glm::normalize(glm::vec3(0, 1, 0)));
        Planet.model = glm::translate(Planet.model, { 15.0f, 0.0f, 0.0f });
        
        Mond2.model = glm::translate(Mond2.model, { -15.0f,5.0f,0.0f });
        Mond2.model = glm::rotate(Mond2.model, glm::radians(rotationy), glm::normalize(glm::vec3(0, 1, 0)));
        Mond2.model = glm::rotate(Mond2.model, glm::radians(rotationx), glm::normalize(glm::vec3(1, 0, 0)));
        Mond2.model = glm::translate(Mond2.model, { 0.0f,-5.0f,0.0f });

        Mond1.model = glm::translate(Mond1.model, { -15.0f,-5.0f,0.0f });
        Mond1.model = glm::rotate(Mond1.model, glm::radians(rotationx), glm::normalize(glm::vec3(1, 0, 0)));
        Mond1.model = glm::rotate(Mond1.model, glm::radians(rotationy), glm::normalize(glm::vec3(0, 1, 0)));
        Mond1.model = glm::translate(Mond1.model, { 0.0f,5.0f,0.0f });

        
        xaxis.model = glm::translate(xaxis.model, { -15.0f,0.0f,0.0f });
        xaxis.model = glm::rotate(xaxis.model, glm::radians(rotationy), glm::normalize(glm::vec3(0, 1, 0)));
        xaxis.model = glm::translate(xaxis.model, { 15.0f,0.0f,0.0f });

        
    }
    else {
        /*
        Monde rotieren um den Planeten
        */
        Mond2.model = glm::translate(Mond2.model, { -15.0f,5.0f,0.0f });
        Mond2.model = glm::rotate(Mond2.model, glm::radians(rotationx), glm::normalize(glm::vec3(1, 0, 0)));
        Mond2.model = glm::translate(Mond2.model, { 15.0f,-5.0f,0.0f });

        Mond1.model = glm::translate(Mond1.model, { -15.0f,-5.0f,0.0f });
        Mond1.model = glm::rotate(Mond1.model, glm::radians(rotationx), glm::normalize(glm::vec3(1, 0, 0)));
        Mond1.model = glm::translate(Mond1.model, { 15.0f,+5.0f,0.0f });

        /*
        Planet rotiert um sich selbst
        */
        Planet.model = glm::rotate(Planet.model, glm::radians(rotationy), glm::normalize(glm::vec3(0, 1, 0)));
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
    case 'a':
        Z_VIEW--;
        initView();
        break;
    case 's':
        Z_VIEW++;
        initView();
        break;
    case 'u':
        /*
        Verschiebt Planet, Planetachse und Monde nach oben, entlang der Planetenachse
        */
        Planet.model = glm::translate(Planet.model, { 0.0f,1.0f,0.0f });
        Planet.render(GL_TRIANGLES, 24, view, projection, program);

        Mond1.model = glm::translate(Mond1.model, { 0.0f,1.0f,0.0f });
        Mond1.render(GL_TRIANGLES, 24, view, projection, program);

        Mond2.model = glm::translate(Mond2.model, { 0.0f,1.0f,0.0f });
        Mond2.render(GL_TRIANGLES, 24, view, projection, program);

        xaxis.model = glm::translate(xaxis.model, { 0.0f,1.0f,0.0f });
        xaxis.render(GL_TRIANGLES, 24, view, projection, program);
        break;
    case 'i':
        /*
        Verschiebt Planet, Planetachse und Monde nach unten, entlang der Planetenachse
        */
        Planet.model = glm::translate(Planet.model, { 0.0f,-1.0f,0.0f });
        Planet.render(GL_TRIANGLES, 24, view, projection, program);

        Mond1.model = glm::translate(Mond1.model, { 0.0f,-1.0f,0.0f });
        Mond1.render(GL_TRIANGLES, 24, view, projection, program);

        Mond2.model = glm::translate(Mond2.model, { 0.0f,-1.0f,0.0f });
        Mond2.render(GL_TRIANGLES, 24, view, projection, program);

        xaxis.model = glm::translate(xaxis.model, { 0.0f,-1.0f,0.0f });
        xaxis.render(GL_TRIANGLES, 24, view, projection, program);
        break;
    case 'g':
        /*
        Stoppt und startet Rotation um die Sonne
        */

        if (g) g = false;
        else g = true;
        break;
    case 'd':
        /*
        Senkt Animationsgeschwindigkeit
        */
        if (n == 1) break;
        n--;
        break;
    case 'f':
        /*
        Erhöht Animationsgeschwindigkeit
        */
        if (n == 4) break;
        n++;
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

