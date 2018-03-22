// ==========================================================================
// $Id: lit_boxes.cpp,v 1.8 2018/03/08 16:23:08 jlang Exp $
// Instaced boxes inspired by the OpenGL programming guide 8th ed. by
// Shreiner et al.
// ==========================================================================
// (C)opyright:
//
//   Jochen Lang
//   SITE, University of Ottawa
//   800 King Edward Ave.
//   Ottawa, On., K1N 6N5
//   Canada.
//   http://www.site.uottawa.ca
//
// Creator: Jochen Lang
// Email:   jlang@site.uottawa.ca
// ==========================================================================
// $Log: lit_boxes.cpp,v $
// Revision 1.8  2018/03/08 16:23:08  jlang
// Update to reflect shape hierarchy
//
// Revision 1.7  2018/03/05 03:18:42  jlang
// Updated to render_shape
//
// Revision 1.6  2017/02/27 20:57:50  jlang
// include algorithm for windows
//
// Revision 1.5  2016/03/12 03:26:26  jlang
// Corrected spot light
//
// Revision 1.4  2014/10/18 14:52:42  jlang
// Added error checking in light.h, switched to radians, added core.
//
// Revision 1.3  2014/02/24 16:45:59  jlang
// Lab 5 post
//
// Revision 1.2  2014/02/22 19:51:39  jlang
// Drawing with indexed materials and 1 light source
//
// Revision 1.1  2014/02/17 18:52:57  jlang
// Draft lab5
//
// Revision 1.4  2014/02/10 20:43:03  jlang
// Created skeleton programs.
//
// Revision 1.3  2014/02/07 22:44:45  jlang
// Merged version between laptop and desktop
//
// Revision 1.2  2014/02/06 23:30:05  jlang
// Instanced rendering with drawElements working - needs better way to pass translation limits.
//
// Revision 1.1  2014/02/03 03:38:45  jlang
// Draft lab check in
//
// ==========================================================================
#define NOMINMAXS
#ifndef _WIN32
#define JOELAPTOP
#else
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <cstdlib>
#include <stack>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>

#ifdef JOELAPTOP
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

// glm types
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
// matrix manipulation
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "box_shape.h"
#include "light.h"
#include "material.h"
#include "sphere.h"
#include "torus.h"
using namespace CSI4130;
using std::cerr;
using std::endl;

// #define UNIFORM

namespace CSI4130 {

// Window dimensions
struct WindowSize {
        GLfloat d_near;
        GLfloat d_far;
        GLint d_widthPixel;
        GLfloat d_width;
        GLint d_heightPixel;
        GLfloat d_height;
        bool d_perspective;
        WindowSize() : d_near(1.0f), d_far(21.0f),
                d_widthPixel(512), d_width(12.5f),
                d_heightPixel(512), d_height(12.5f)
        {
        }
};

/*
 * Helper structure holding the locations of the uniforms to transform matrices
 */
struct Transformations {
        GLint locP;
        GLint locVM;
        GLint locMM; // per instance model matrix
        Transformations() : locP(-1), locVM(-1), locMM(-1) {
        }
};

struct Attributes {
        GLint locPos;
        GLint locNorm;
        GLint locColor;
        Attributes() : locPos(-1), locNorm(-1), locColor(-1) {
        }
};

struct ControlParameter {
        bool d_spot;
        bool d_attenuation;
        ControlParameter() : d_spot(false), d_attenuation(false) {
        }
};

/** Global variables */
// const int g_numBoxes = 24;
// BoxShape g_boxShape;
const int g_numSpheres = 30;
Sphere g_sphereShape;
// const int g_numTorus = 12;
// Sphere g_torusShape;

//GLuint g_box_ebo;
//GLuint g_box_vao;
GLuint g_sphere_ebo;
GLuint g_sphere_vao;
//GLuint g_torus_ebo;
//GLuint g_torus_vao;

// load both LAFORTUNE and BLINNPHONG programs
// so we can switch between
#define NUM_PROGRAMS 2
GLuint g_program[NUM_PROGRAMS];
int g_programIndex = 0; // default is 0 for blinn_phong

Transformations g_tfm[NUM_PROGRAMS];

Attributes g_attrib[NUM_PROGRAMS];

WindowSize g_winSize;

LightArray g_lightArray;
MaterialArray g_matArray;
GLfloat g_lightAngle = 0.0f;
GLfloat g_camX = 0.0f, g_camY = 0.0f;
ControlParameter g_control;

// keep track of light indices
int g_cLight = 0;

// keep track of lighting model
#define NUM_MODELS 2
enum lightModel { BLINN_PHONG, LAFORTUNE };
lightModel g_lightModel = BLINN_PHONG;


void initMaterial() {

        Material mat;
        // material 0 - blue plastic
        mat.d_ambient = glm::vec4(0.02f, 0.02f, 0.05f, 1.0f);
        mat.d_diffuse = glm::vec4(0.2f, 0.2f, 0.5f, 1.0f);
        mat.d_specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        mat.d_shininess = 32.0f;
        g_matArray.append( mat );
        // material 1 - turquoise
        mat.d_ambient = glm::vec4(0.02f, 0.05f, 0.04f, 1.0f);
        mat.d_diffuse = glm::vec4(0.2f, 0.5f, 0.4f, 1.0f);
        mat.d_specular = glm::vec4(0.3f, 0.4f, 0.35f, 1.0f);
        mat.d_shininess = 12.5f;
        g_matArray.append( mat );
        // material 2 - ruby
        mat.d_ambient = glm::vec4(0.06f, 0.005f, 0.005f, 1.0f);
        mat.d_diffuse = glm::vec4(0.6f, 0.05f, 0.05f, 1.0f);
        mat.d_specular = glm::vec4(0.35f, 0.2f, 0.2f, 1.0f);
        mat.d_shininess = 76.5f;
        g_matArray.append( mat );
        // material 3 - jade
        mat.d_ambient = glm::vec4(0.035f, 0.045f, 0.04f, 1.0f);
        mat.d_diffuse = glm::vec4(0.35f, 0.45f, 0.4f, 1.0f);
        mat.d_specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        mat.d_shininess = 8;
        g_matArray.append( mat );

        // material 4 - blue rubber
        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0425f, 0.0698f, 0.0957f, 1.0f);
        mat.d_specular = glm::vec4(0.00533f, 0.00471f, 0.00333, 1.0f);
        mat.d_shininess = 43.6f;
        g_matArray.append( mat);

        // material 5 - Brass
        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0382f, 0.0272f, 0.0119f, 1.0f);
        mat.d_specular = glm::vec4(0.0367f, 0.015f, 0.00537f, 1.0f);
        mat.d_shininess = 3.16e+004;
        g_matArray.append( mat);

        // material 6 -- metal
        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0695f, 0.0628f, 0.0446f, 1.0f);
        mat.d_specular = glm::vec4(0.0742f, 0.0615f, 0.0412f, 1.0f);
        mat.d_shininess = 75.0f;
        g_matArray.append( mat);

        // Lafortune materials
        //Blue rubber
        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0464f, 0.0736f, 0.0986f, 1.0f);
        mat.d_specular = glm::vec4(0.291f, 0.239f, 0.159f, 1.0f);
        mat.d_shininess = 32.6f;
        mat.d_Kxy = -0.635f;
        mat.d_Kz = 0.44f;
        g_matArray.append( mat);

        //Brass
        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0387f, 0.0273f, 0.0123f, 1.0f);
        mat.d_specular = glm::vec4(0.118f, 0.0479f, 0.0172f, 1.0f);
        mat.d_shininess = 1.07e+004;
        mat.d_Kxy = -0.577f;
        mat.d_Kz = 0.577f;
        g_matArray.append( mat);

        //Metallic-silver

        mat.d_ambient = glm::vec4(0.0f);
        mat.d_diffuse = glm::vec4(0.0552f, 0.05f, 0.0359f, 1.0f);
        mat.d_specular = glm::vec4(0.434f, 0.363f, 0.243f, 1.0f);
        mat.d_shininess = 21.4;
        mat.d_Kxy = -0.587f;
        mat.d_Kz = 0.559f;
        g_matArray.append( mat);
        return;
}


void initLight() {

        // light 0 is controllable default
        g_lightArray.append( LightSource() );
        // Place the current light source at a radius from the camera
        LightSource light = g_lightArray.get(0);
        light.d_position =
                glm::vec4(cos(g_lightAngle)*g_winSize.d_width,
                          sin(g_lightAngle)*g_winSize.d_width,
                          20.0f, // * static_cast<GLfloat>( !light.d_local ),
                          static_cast<GLfloat>(light.d_local));
        g_lightArray.set(0, light);

        // a directional light source from the center of the upper-left edge of the viewing volume
        LightSource l1 = LightSource(
                glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // ambient
                glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // diffuse
                glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // specular
                1, // enabled
                0, // pointLight
                0, // spotLight
                10.0f, // strength
                glm::vec3(1.0f, 0.0f, 0.0f), // not used -- but set in case toggled
                (GLfloat)8.0f, // spot exponent
                (GLfloat)80.0f, // cutoff > 180 - not a spotlight
                // no attenuation for direction light
                (GLfloat)1.0f, //a1
                (GLfloat)0.0f, //a2 attenuation constants
                (GLfloat)0.0f, //a3
                glm::vec4(-1.0,
                          1.0,
                          0.0f, // * static_cast<GLfloat>( !light.d_local ),
                          0.0)); // z=0 for direction light

        g_lightArray.append(l1);

        // a point light source on the upper - right rear corner of the viewing volume
        LightSource l2 = LightSource(
                glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // ambient
                glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // diffuse
                glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // specular
                1, // enabled
                1, // pointLight
                0, // spotLight
                10.0f, // strength
                glm::vec3(-1.0f, -1.0f, 1.0f), // spot direction
                (GLfloat)8.0f, // spot exponent
                (GLfloat)80.0f, // cutoff > 180 - not a spotlight
                (GLfloat)1.0f, //a1
                (GLfloat)0.0f, //a2 attenuation constants
                (GLfloat)0.0f, //a3
                glm::vec4(1.0f, 1.0f, -1.0f, 1.0f)); // z=1 for point light

        g_lightArray.append(l2);

        return;
}


void init(void)
{
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glEnable( GL_DEPTH_TEST );
        errorOut();

        // Make sure that our shaders run
        int major, minor;
        getGlVersion( major, minor );
        cerr << "Running OpenGL "<< major << "." << minor << endl;
        if ( major < 3 || (major==3 && minor<3)) {
                cerr << "No OpenGL 3.3 or higher" <<endl;
                exit(-1);
        }

        // init lights and material in our global arrays
        initLight();
        initMaterial();

        // ensure that we have enough transforms
        g_sphereShape.updateTransforms(g_numSpheres,
                                       glm::vec3(-g_winSize.d_width/2.0f,
                                                 -g_winSize.d_height/2.0f,
                                                 -(g_winSize.d_far - g_winSize.d_near)/2.0f),
                                       glm::vec3( g_winSize.d_width/2.0f,
                                                  g_winSize.d_height/2.0f,
                                                  (g_winSize.d_far - g_winSize.d_near)/2.0f));

        // for all NUM_PROGRAMS
        for (int i=0; i < NUM_PROGRAMS; ++i) {

                // Load shaders
                vector<GLuint> sHandles;
                GLuint handle;
                Shader objectShader;
                switch (static_cast<lightModel>(i)) {
                case LAFORTUNE:
                        if (!objectShader.load("../lafortune.vs", GL_VERTEX_SHADER)) {
                                objectShader.installShader(handle, GL_VERTEX_SHADER);
                                Shader::compile(handle);
                                sHandles.push_back(handle);
                        }
                        if (!objectShader.load("../lafortune.fs", GL_FRAGMENT_SHADER)) {
                                objectShader.installShader(handle, GL_FRAGMENT_SHADER);
                                Shader::compile(handle);
                                sHandles.push_back(handle);
                        }
                        break;
                case BLINN_PHONG:
                default:
                        if (!objectShader.load("../blinnphong.vs", GL_VERTEX_SHADER)) {
                                objectShader.installShader(handle, GL_VERTEX_SHADER);
                                Shader::compile(handle);
                                sHandles.push_back(handle);
                        }
                        if (!objectShader.load("../blinnphong.fs", GL_FRAGMENT_SHADER)) {
                                objectShader.installShader(handle, GL_FRAGMENT_SHADER);
                                Shader::compile(handle);
                                sHandles.push_back(handle);
                        }
                        break;
                }
                cerr << "No of handles: " << sHandles.size() << endl;
                Shader::installProgram(sHandles, g_program[i]);
                errorOut();

                // find the locations of uniforms and attributes. Store them in a
                // global structure for later access

                // Activate program in order to be able to get uniform and attribute locations
                glUseProgram(g_program[i]);
                errorOut();
                // vertex attributes
                g_attrib[i].locPos = glGetAttribLocation(g_program[i], "position");
                g_attrib[i].locNorm = glGetAttribLocation(g_program[i], "normal");
                g_attrib[i].locColor = glGetAttribLocation(g_program[i], "color");
                // transform uniforms and attributes
                g_tfm[i].locMM = glGetAttribLocation( g_program[i], "ModelMatrix");
                g_tfm[i].locVM = glGetUniformLocation( g_program[i], "ViewMatrix");
                g_tfm[i].locP = glGetUniformLocation( g_program[i], "ProjectionMatrix");
                errorOut();

                // Generate a VAO
                glGenVertexArrays(1, &g_sphere_vao );
                glBindVertexArray( g_sphere_vao );

                // Element array buffer object
                glGenBuffers(1, &g_sphere_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sphere_ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             sizeof(GLushort) * g_sphereShape.getNIndices(),
                             g_sphereShape.getIndicies(), GL_STATIC_DRAW);
                errorOut();

                GLuint vbo;
                glGenBuffers( 1, &vbo );
                errorOut();
                glBindBuffer(GL_ARRAY_BUFFER, vbo );
                glBufferData(GL_ARRAY_BUFFER,
                             sizeof(GLfloat) * g_sphereShape.getNPoints(),
                             g_sphereShape.getVertices(), GL_STATIC_DRAW);
                // pointer into the array of vertices which is now in the VAO
                glVertexAttribPointer(g_attrib[i].locPos, 3, GL_FLOAT, GL_FALSE, 0, 0 );
                glEnableVertexAttribArray(g_attrib[i].locPos);
                errorOut();

                // Normal buffer
                if ( g_attrib[i].locNorm >= 0 ) { // May be optimized away if not used in vertex shader
                        GLuint nbo;
                        glGenBuffers( 1, &nbo );
                        errorOut();
                        glBindBuffer(GL_ARRAY_BUFFER, nbo );
                        glBufferData(GL_ARRAY_BUFFER,
                                     sizeof(GLfloat) * g_sphereShape.getNPoints(),
                                     g_sphereShape.getNormals(), GL_STATIC_DRAW);
                        // pointer into the array of vertices which is now in the VAO
                        glVertexAttribPointer(g_attrib[i].locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0 );
                        glEnableVertexAttribArray(g_attrib[i].locNorm);
                        errorOut();
                }
                // Color buffer
                if ( g_attrib[i].locColor >= 0 ) {
                        g_sphereShape.updateColors(g_numSpheres); // ensure that we have enough colors
                        GLuint cbo;
                        glGenBuffers(1, &cbo);
                        glBindBuffer(GL_ARRAY_BUFFER, cbo);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * g_sphereShape.getNColors(),
                                     g_sphereShape.d_colors, GL_DYNAMIC_DRAW);
                        glVertexAttribPointer(g_attrib[i].locColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
                        glEnableVertexAttribArray(g_attrib[i].locColor);
                        // Ensure the colors are used per instance and not for each vertex
                        glVertexAttribDivisor(g_attrib[i].locColor, 1);
                        errorOut();
                }

                // Matrix attribute
                if ( g_tfm[i].locMM >= 0 ) {
                        GLuint mmbo;
                        glGenBuffers(1, &mmbo);
                        glBindBuffer(GL_ARRAY_BUFFER, mmbo);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_sphereShape.getNTransforms(),
                                     g_sphereShape.d_tfms, GL_DYNAMIC_DRAW);
                        // Need to set each column separately.
                        for (int j = 0; j < 4; ++j) {
                                // Set up the vertex attribute
                                glVertexAttribPointer(g_tfm[i].locMM + j, // Location
                                                      4, GL_FLOAT, GL_FALSE, // Column with four floats
                                                      sizeof(glm::mat4), // Stride for next matrix
                                                      (void *)(sizeof(GLfloat) * 4 * j)); // Offset for ith column
                                glEnableVertexAttribArray(g_tfm[i].locMM + j);
                                // Matrix per instance
                                glVertexAttribDivisor(g_tfm[i].locMM  + j, 1);
                        }
                        errorOut();
                }

                // Light source uniforms
                g_lightArray.setLights(g_program[i]);
                errorOut();
                g_lightArray.setNLights(g_program[i]);
                errorOut();

                // material uniforms
                // Generate an uniform buffer object
                GLuint ubo;
                glGenBuffers(1, &ubo);
                glBindBuffer(GL_UNIFORM_BUFFER, ubo);
                // reserve the memory
                glBufferData(GL_UNIFORM_BUFFER, g_matArray.getSize(), 0,
                             GL_STATIC_DRAW );
                // copy the data to OpenGL
                g_matArray.setMaterialsUBO(ubo);
                errorOut();
                // Now link the buffer object to the material uniform block
                GLuint bI = glGetUniformBlockIndex(g_program[i], "MaterialBlock" );
                if ( bI >= 0 ) {
                        glUniformBlockBinding( g_program[i], bI, 0);
                        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
                }
                errorOut();

                // set the projection matrix with a uniform
                glm::mat4 Projection =
                        glm::ortho( -g_winSize.d_width/2.0f, g_winSize.d_width/2.0f,
                                    -g_winSize.d_height/2.0f, g_winSize.d_height/2.0f,
                                    g_winSize.d_near, g_winSize.d_far );

                glUniformMatrix4fv(g_tfm[i].locP, 1, GL_FALSE, glm::value_ptr(Projection));
                errorOut();


        }

        glUseProgram(g_program[g_programIndex]); // start  with blinnphong?
}


void display(void)
{
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // update light positions to control movement
        for (int i = 0; i < g_lightArray.size(); ++i) {
                g_lightArray.setPosition(g_program[g_programIndex], i);
        }

        // Instead of moving the coordinate system into the scene,
        // use lookAt -- use the center of the viewing volume as the reference coordinates
        glm::mat4 ModelView =
                glm::lookAt( glm::vec3(g_camX, g_camY, -(g_winSize.d_far+g_winSize.d_near)/2.0f ),
                             glm::vec3(0, 0, 0), // at is the center of the cube
                             glm::vec3(0, 1.0f, 0 )); // y is up
        // Update uniform for this drawing
        glUniformMatrix4fv(g_tfm[g_programIndex].locVM, 1, GL_FALSE, glm::value_ptr(ModelView));
        // VAO is still bound - to be clear bind again
        glBindVertexArray(g_sphere_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sphere_ebo);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(g_sphereShape.getRestart());
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, g_sphereShape.getNIndices(),
                                GL_UNSIGNED_SHORT, 0, g_numSpheres);
        errorOut();
        // swap buffers
        glFlush();
        glutSwapBuffers();
}


/**
 * OpenGL reshape function - main window
 */
void reshape( GLsizei _width, GLsizei _height ) {
        GLfloat minDim = std::min(g_winSize.d_width,g_winSize.d_height);
        // adjust the view volume to the correct aspect ratio
        if ( _width > _height ) {
                g_winSize.d_width = minDim  * (GLfloat)_width/(GLfloat)_height;
                g_winSize.d_height = minDim;
        } else {
                g_winSize.d_width = minDim;
                g_winSize.d_height = minDim * (GLfloat)_height/(GLfloat)_width;
        }
        glm::mat4 Projection;
        if ( g_winSize.d_perspective ) {
                Projection = glm::frustum( -g_winSize.d_width/2.0f, g_winSize.d_width/2.0f,
                                           -g_winSize.d_height/2.0f, g_winSize.d_height/2.0f,
                                           g_winSize.d_near, g_winSize.d_far );
        } else {
                Projection = glm::ortho( -g_winSize.d_width/2.0f, g_winSize.d_width/2.0f,
                                         -g_winSize.d_height/2.0f, g_winSize.d_height/2.0f,
                                         g_winSize.d_near, g_winSize.d_far );
        }
        // change projection for both programs
        for (int i=0; i< NUM_PROGRAMS; ++i) {
          glUniformMatrix4fv(g_tfm[i].locP, 1, GL_FALSE, glm::value_ptr(Projection));
        }

        g_winSize.d_widthPixel = _width;
        g_winSize.d_heightPixel = _height;
        // reshape our viewport
        glViewport( 0, 0,
                    g_winSize.d_widthPixel,
                    g_winSize.d_heightPixel );
}


void keyboard (unsigned char key, int x, int y)
{
        LightSource light;

        switch (key) {
        case 27:
        case 'q':
                exit(0);
                break;
        // switch lights
        case 'i':
                g_cLight = ++g_cLight % g_lightArray.size();
                cerr << "Toggle Light " << g_cLight << endl;
                break;
        // turn light on and off
        case 'o':
                light = g_lightArray.get( g_cLight );
                light.d_enabled = !light.d_enabled;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "Light " << g_cLight << " is "<< light.d_enabled << endl;
                break;
        // controls light 0 pos vec
        case '+':
                g_lightAngle += 0.1f;
                cerr << "lightAngle: " << g_lightAngle << endl;
                break;
        case '-':
                g_lightAngle -= 0.1f;
                cerr << "lightAngle: " << g_lightAngle << endl;
                break;
        case 'P':
                // switch to perspective
                g_winSize.d_perspective = true;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                cerr << "Switch to persepective" << endl;
                break;
        case 'p':
                // switch to perspective
                g_winSize.d_perspective = false;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                cerr << "Switch to ortho" << endl;
                break;
        case 'Z':
                // increase near plane
                g_winSize.d_near += 0.1f;
                g_winSize.d_far += 0.1f;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                cerr << "z-near: " << g_winSize.d_near << endl;
                break;
        case 'z':
                // decrease near plane
                if ( g_winSize.d_near > 0.1f ) {
                        g_winSize.d_near -= 0.1f;
                        g_winSize.d_far -= 0.1f;
                }
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                cerr << "z-near: " << g_winSize.d_near << endl;
                break;
        case 'L':
                break;
        case 'l':
                break;
        // ambient term on/off
        case 'A':
                light = g_lightArray.get( g_cLight );
                light.d_ambient = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f);
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "Light " << g_cLight << " ambient on." << endl;
                break;
        case 'a':
                light = g_lightArray.get( g_cLight );
                light.d_ambient = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f);
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "Light " << g_cLight << " ambient off." << endl;
                break;
        // directional/point light
        case 'D':
                light = g_lightArray.get( g_cLight );
                light.d_local = false;
                light.d_position.w = 0.0f; // flag needed in vertex shader
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                g_lightArray.setPosition(g_program[g_programIndex], g_cLight );
                cerr << "Toggle light " << g_cLight << " pointLight off." << endl;
                break;
        case 'd':
                light = g_lightArray.get( g_cLight );
                light.d_local = true;
                light.d_position.w = 1.0f; // flag needed in vertex shader
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                g_lightArray.setPosition(g_program[g_programIndex], g_cLight );
                cerr << "Toggle light " << g_cLight << " pointLight on." << endl;
                break;
        // spot light on/off
        case 'S':
                light = g_lightArray.get( g_cLight );
                light.d_local = true; // No directional lighting
                light.d_spotLight = true;
                light.d_spot_cutoff = 80.0f;
                g_control.d_spot = true;
                g_control.d_attenuation = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "SpotLight on with params: "
                     << g_cLight << " spot exponent: " << light.d_spot_exponent
                     << " cutoff " << light.d_spot_cutoff << endl;
                break;
        case 's':
                light = g_lightArray.get( g_cLight );
                light.d_spotLight = false;
                light.d_spot_cutoff = 180.0f;
                g_control.d_spot = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "SpotLight off." << endl;
                break;
        // attenuation on/off
        case 'T':
                light = g_lightArray.get( g_cLight );
                light.d_constant_attenuation = 1.0f;
                light.d_linear_attenuation = 0.0f;
                light.d_quadratic_attenuation = 0.0f;
                g_control.d_attenuation = true;
                g_control.d_spot = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "Light: " << g_cLight << " attenuation on.";
                cerr << " " << light.d_constant_attenuation
                     << " " << light.d_linear_attenuation
                     << " " << light.d_quadratic_attenuation << endl;
                break;
        case 't':
                light = g_lightArray.get( g_cLight );
                light.d_constant_attenuation = 1.0f;
                light.d_linear_attenuation = 0.0f;
                light.d_quadratic_attenuation = 0.0f;
                g_control.d_attenuation = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                cerr << "Light: " << g_cLight << " attenuation off.";
                cerr << " " << light.d_constant_attenuation
                     << " " << light.d_linear_attenuation
                     << " " << light.d_quadratic_attenuation << endl;
                break;

        case 'b':
//        enum lightModel { BLINN_PHONG, LAFORTUNE };
//        lightModel g_lightModel = BLINN_PHONG;
                if (g_lightModel != BLINN_PHONG) {
                        g_lightModel = BLINN_PHONG;
                        g_programIndex = 0;
                        reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                        glUseProgram(g_program[g_programIndex]);
                }
                break;
        case 'B':
                if (g_lightModel != LAFORTUNE) {
                        g_lightModel = LAFORTUNE;
                        g_programIndex = 1;
                        reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                        glUseProgram(g_program[g_programIndex]);
                }
                break;

        // predefined camera positions
        case '0':
                g_camX = 0.0f, g_camY = 0.0f;
                break;
        case '1':
                g_camX = g_winSize.d_width/6.0f, g_camY = -g_winSize.d_height/6.0f;
                break;
        case '2':
                g_camX = g_winSize.d_width/6.0f, g_camY = g_winSize.d_height/6.0f;
                break;
        case '3':
                g_camX = -g_winSize.d_width/6.0f, g_camY = g_winSize.d_height/6.0f;
                break;
        case '4':
                g_camX = -g_winSize.d_width/6.0f, g_camY = -g_winSize.d_height/6.0f;
                break;
        default:
                break;
        }
        glutPostRedisplay();
}
}

void specialkeys( int key, int x, int y )
{
        LightSource light;

        switch (key) {
        case GLUT_KEY_LEFT:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_exponent = std::max(light.d_spot_exponent-0.1f, 0.0f);
                        cerr << "Spot params for light: " << g_cLight
                             << " exponent: " <<  light.d_spot_exponent
                             << " cutoff: " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_quadratic_attenuation =
                                std::max(light.d_quadratic_attenuation-0.0005f, 0.0f);
                        cerr << "Attenuation for light: " << g_cLight
                             << " constant: " <<  light.d_constant_attenuation
                             << " linear: " << light.d_linear_attenuation
                             << " quadratic: " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                break;
        case GLUT_KEY_RIGHT:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_exponent =
                                std::min( light.d_spot_exponent+0.1f, 128.0f);
                        cerr << "Spot params for light: " << g_cLight
                             << " exponent: " <<  light.d_spot_exponent
                             << " cutoff: " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_quadratic_attenuation += 0.0005f;
                        cerr << "Attenuation for light: " << g_cLight
                             << " constant: " <<  light.d_constant_attenuation
                             << " linear: " << light.d_linear_attenuation
                             << " quadratic: " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                break;
        case GLUT_KEY_UP:
                light = g_lightArray.get( g_cLight );
                if (!g_control.d_spot && !g_control.d_attenuation) {
                        light.d_strength = std::min(light.d_strength+0.1f, 100.0f);
                        cerr << "Light: " << g_cLight
                             << " strength: " <<  light.d_strength << endl;
                }
                if (g_control.d_spot) {
                        light.d_spot_cutoff = std::min(light.d_spot_cutoff+1.0f, 180.0f);
                        cerr << "Spot params for light: " << g_cLight
                             << " exponent: " <<  light.d_spot_exponent
                             << " cutoff: " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_linear_attenuation = light.d_linear_attenuation+0.001f;
                        cerr << "Attenuation for light: " << g_cLight
                             << " constant: " <<  light.d_constant_attenuation
                             << " linear: " << light.d_linear_attenuation
                             << " quadratic: " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                break;
        case GLUT_KEY_DOWN:
                light = g_lightArray.get( g_cLight );
                if (!g_control.d_spot && !g_control.d_attenuation) {
                        light.d_strength = std::max(light.d_strength-0.1f, 0.0f);
                        cerr << "Light: " << g_cLight
                             << " strength: " <<  light.d_strength << endl;
                }
                if (g_control.d_spot) {
                        light.d_spot_cutoff = std::max(light.d_spot_cutoff-1.0f, 0.0f);
                        cerr << "Spot params for light: " << g_cLight
                             << " exponent: " <<  light.d_spot_exponent
                             << " cutoff: " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_linear_attenuation =
                                std::max(light.d_linear_attenuation-0.001f, 0.0f);
                        cerr << "Attenuation for light: " << g_cLight
                             << " constant: " <<  light.d_constant_attenuation
                             << " linear: " << light.d_linear_attenuation
                             << " quadratic: " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program[g_programIndex], g_cLight );
                break;
        case GLUT_KEY_PAGE_UP:
                g_winSize.d_height += 0.2f;
                g_winSize.d_width += 0.2f;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                break;
        case GLUT_KEY_PAGE_DOWN:
                g_winSize.d_height -= 0.2f;
                g_winSize.d_width -= 0.2f;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                break;
        default: break;
        }
        glutPostRedisplay();
}


int main(int argc, char** argv) {
  #ifdef JOELAPTOP
// annoying version stuff
        glewExperimental = GL_TRUE;
        cerr << "Set GLEW Experimental Version" <<endl;
        errorOut();
        cerr << "Specity Context Version: 4,5"<<endl;
        glutInitContextVersion(4,5);
        errorOut();
  #endif

        glutInit(&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize (800, 600);
        glutInitWindowPosition (0, 0);
        glutCreateWindow (argv[0]);
        GLenum err = glewInit();
        if (GLEW_OK != err) {
                /* Problem: glewInit failed, something is seriously wrong. */
                cerr << "Error: " << glewGetErrorString(err) << endl;
                return -1;
        }
        cerr << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

        cerr << "Subidividing Spheres" << endl;

        // make spheres prettier before sending to buffer.
        // warning: setting N>7 will take a looong time to load, and possibly crash.
        for (int i = 0; i < 5; ++i) {
                g_sphereShape.subdivide();
        }

        cerr << "Before init" << endl;
        init();
        cerr << "After init" << endl;
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutSpecialFunc(specialkeys);
        glutKeyboardFunc(keyboard);
        glutMainLoop();
        return 0;
}
