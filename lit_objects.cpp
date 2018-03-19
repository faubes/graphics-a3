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
const int g_numSpheres = 32;
Sphere g_sphereShape;
// const int g_numTorus = 12;
// Sphere g_torusShape;

//GLuint g_box_ebo;
//GLuint g_box_vao;
GLuint g_sphere_ebo;
GLuint g_sphere_vao; 
//GLuint g_torus_ebo;
//GLuint g_torus_vao;
GLuint g_program;
Transformations g_tfm;
Attributes g_attrib;
WindowSize g_winSize;
int g_cLight = 0;
LightArray g_lightArray;
MaterialArray g_matArray;
GLfloat g_lightAngle = 0.0f;
GLfloat g_camX = 0.0f, g_camY = 0.0f;
ControlParameter g_control;

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
        return;
}


void initLight( int _nLights ) {
   
	// light 0 is controllable default
    g_lightArray.append( LightSource() );

	// a directional light source from the center of the upper-left edge of the viewing volume
	LightSource l1 = LightSource(
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // ambient
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // diffuse
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // specular
		0,  // pointlight?
		glm::vec3(0.0f),  // spot direction not used here
		(GLfloat)1.0f, // spot exponent
		(GLfloat)0.0f,  // cutoff
		(GLfloat)0.2f, //a1
		(GLfloat)0.35f, //a2 attenuation constants
		(GLfloat)0.4f, //a3
		glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)); // z=0 for direction light
	
	g_lightArray.append(l1);
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
        initLight(2);
        initMaterial();

        // Load shaders
        vector<GLuint> sHandles;
        GLuint handle;
        Shader boxes;
        if ( !boxes.load("lit_boxes_sol.vs", GL_VERTEX_SHADER )) {
                boxes.installShader( handle, GL_VERTEX_SHADER );
                Shader::compile( handle );
                sHandles.push_back( handle );
        }
        if ( !boxes.load("lit_boxes_sol.fs", GL_FRAGMENT_SHADER )) {
                boxes.installShader( handle, GL_FRAGMENT_SHADER );
                Shader::compile( handle );
                sHandles.push_back( handle );
        }
        cerr << "No of handles: " << sHandles.size() << endl;
        Shader::installProgram(sHandles, g_program);
        errorOut();

        // find the locations of uniforms and attributes. Store them in a
        // global structure for later access

        // Activate program in order to be able to get uniform and attribute locations
        glUseProgram(g_program);
        errorOut();
        // vertex attributes
        g_attrib.locPos = glGetAttribLocation(g_program, "position");
        g_attrib.locNorm = glGetAttribLocation(g_program, "normal");
        g_attrib.locColor = glGetAttribLocation(g_program, "color");
        // transform uniforms and attributes
        g_tfm.locMM = glGetAttribLocation( g_program, "ModelMatrix");
        g_tfm.locVM = glGetUniformLocation( g_program, "ViewMatrix");
        g_tfm.locP = glGetUniformLocation( g_program, "ProjectionMatrix");
        errorOut();

		// make spheres prettier
		for (int i = 0; i < 3; ++i) {
			g_sphereShape.subdivide();
		}

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
                     sizeof(GLfloat) * 3 * g_sphereShape.getNPoints(),
                     g_sphereShape.getVertices(), GL_STATIC_DRAW);
        // pointer into the array of vertices which is now in the VAO
        glVertexAttribPointer(g_attrib.locPos, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray(g_attrib.locPos);
        errorOut();

        // Normal buffer
        if ( g_attrib.locNorm >= 0 ) { // May be optimized away if not used in vertex shader
                GLuint nbo;
                glGenBuffers( 1, &nbo );
                errorOut();
                glBindBuffer(GL_ARRAY_BUFFER, nbo );
                glBufferData(GL_ARRAY_BUFFER,
                             sizeof(GLfloat) * 3 * g_sphereShape.getNPoints(),
                             g_sphereShape.getNormals(), GL_STATIC_DRAW);
                // pointer into the array of vertices which is now in the VAO
                glVertexAttribPointer(g_attrib.locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0 );
                glEnableVertexAttribArray(g_attrib.locNorm);
                errorOut();
        }
        // Color buffer
        if ( g_attrib.locColor >= 0 ) {
                g_sphereShape.updateColors(g_numSpheres); // ensure that we have enough colors
                GLuint cbo;
                glGenBuffers(1, &cbo);
                glBindBuffer(GL_ARRAY_BUFFER, cbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * g_sphereShape.getNColors(),
					g_sphereShape.d_colors, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(g_attrib.locColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(g_attrib.locColor);
                // Ensure the colors are used per instance and not for each vertex
                glVertexAttribDivisor(g_attrib.locColor, 1);
                errorOut();
        }
        // ensure that we have enough transforms
        g_sphereShape.updateTransforms(g_numSpheres,
                                    glm::vec3(-g_winSize.d_width/2.0f,
                                              -g_winSize.d_height/2.0f,
                                              -(g_winSize.d_far - g_winSize.d_near)/2.0f),
                                    glm::vec3( g_winSize.d_width/2.0f,
                                               g_winSize.d_height/2.0f,
                                               (g_winSize.d_far - g_winSize.d_near)/2.0f));
        // Matrix attribute
        if ( g_tfm.locMM >= 0 ) {
                GLuint mmbo;
                glGenBuffers(1, &mmbo);
                glBindBuffer(GL_ARRAY_BUFFER, mmbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_sphereShape.getNTransforms(),
                             g_sphereShape.d_tfms, GL_DYNAMIC_DRAW);
                // Need to set each column separately.
                for (int i = 0; i < 4; ++i) {
                        // Set up the vertex attribute
                        glVertexAttribPointer(g_tfm.locMM + i, // Location
                                              4, GL_FLOAT, GL_FALSE, // Column with four floats
                                              sizeof(glm::mat4), // Stride for next matrix
                                              (void *)(sizeof(GLfloat) * 4 * i)); // Offset for ith column
                        glEnableVertexAttribArray(g_tfm.locMM + i);
                        // Matrix per instance
                        glVertexAttribDivisor(g_tfm.locMM  + i, 1);
                }
                errorOut();
        }
        // Light source uniforms
        g_lightArray.setLights(g_program);
        errorOut();
        // Could use material uniforms
#ifdef UNIFORM
        g_matArray.setMaterials(g_program);
        errorOut();

#else
		// or an uniform buffer object
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
        GLuint bI = glGetUniformBlockIndex(g_program, "MaterialBlock" );
        if ( bI >= 0 ) {
                glUniformBlockBinding( g_program, bI, 0);
                glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
        }
        errorOut();
#endif

        // set the projection matrix with a uniform
        glm::mat4 Projection =
                glm::ortho( -g_winSize.d_width/2.0f, g_winSize.d_width/2.0f,
                            -g_winSize.d_height/2.0f, g_winSize.d_height/2.0f,
                            g_winSize.d_near, g_winSize.d_far );
        glUniformMatrix4fv(g_tfm.locP, 1, GL_FALSE, glm::value_ptr(Projection));
        errorOut();
}


void display(void)
{
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Place the current light source at a radius from the camera
        LightSource light = g_lightArray.get( g_cLight );
#define DEBUG_DISPLAY
#ifdef DEBUG_DISPLAY
        cerr << cos(g_lightAngle)*g_winSize.d_width << "," <<
        sin(g_lightAngle)*g_winSize.d_width << "," <<
        //static_cast<GLfloat>( !light.d_pointLight )
        20.0f << "," <<
        static_cast<GLfloat>( light.d_pointLight) << endl;
#endif
        std::ostringstream os;
        os << "lightPosition[" << g_cLight << "]";
        std::string varName = os.str();
        GLuint locLightPos = glGetUniformLocation(g_program, varName.c_str());
#ifdef DEBUG_DISPLAY
        cerr << "Location " << varName << " : " << locLightPos << endl;
#endif
        glm::vec4 lightPos =
                glm::vec4( cos(g_lightAngle)*g_winSize.d_width,
                           sin(g_lightAngle)*g_winSize.d_width,
                           20.0f, // * static_cast<GLfloat>( !light.d_pointLight ),
                           static_cast<GLfloat>( light.d_pointLight ));
        glProgramUniform4fv(g_program, locLightPos, 1, glm::value_ptr(lightPos));
        errorOut();

		// sloppy but... for now.
		LightSource light2 = g_lightArray.get(g_cLight +1 );
		os.clear();
		os.str("");
		os << "lightPosition[" << g_cLight + 1 << "]";
		varName = os.str();
		GLuint locLightPos2 = glGetUniformLocation(g_program, varName.c_str());
#ifdef DEBUG_DISPLAY
		cerr << "Location " << varName << " : " << locLightPos << endl;
#endif
		glm::vec4 lightPos2 =
			glm::vec4(g_winSize.d_width,
				g_winSize.d_width,
				1.0f, // * static_cast<GLfloat>( !light.d_pointLight ),
				static_cast<GLfloat>(light.d_pointLight));
		glProgramUniform4fv(g_program, locLightPos2, 1, glm::value_ptr(lightPos2));
		errorOut();

        // Instead of moving the coordinate system into the scene,
        // use lookAt -- use the center of the viewing volume as the reference coordinates
        glm::mat4 ModelView =
                glm::lookAt( glm::vec3(g_camX, g_camY, -(g_winSize.d_far+g_winSize.d_near)/2.0f ),
                             glm::vec3(0, 0, 0), // at is the center of the cube
                             glm::vec3(0, 1.0f, 0 )); // y is up
        // Update uniform for this drawing
        glUniformMatrix4fv(g_tfm.locVM, 1, GL_FALSE, glm::value_ptr(ModelView));
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
        glUniformMatrix4fv(g_tfm.locP, 1, GL_FALSE, glm::value_ptr(Projection));
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
        case '+':
                g_lightAngle += 0.1f;
                break;
        case '-':
                g_lightAngle -= 0.1f;
                break;
        case 'P':
                // switch to perspective
                g_winSize.d_perspective = true;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                break;
        case 'p':
                // switch to perspective
                g_winSize.d_perspective = false;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                break;
        case 'Z':
                // increase near plane
                g_winSize.d_near += 0.1f;
                g_winSize.d_far += 0.1f;
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
                break;
        case 'z':
                // decrease near plane
                if ( g_winSize.d_near > 0.1f ) {
                        g_winSize.d_near -= 0.1f;
                        g_winSize.d_far -= 0.1f;
                }
                reshape( g_winSize.d_widthPixel, g_winSize.d_heightPixel );
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
                g_lightArray.setLight(g_program, g_cLight );
                break;
        case 'a':
                light = g_lightArray.get( g_cLight );
                light.d_ambient = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f);
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        // directional/point light
        case 'D':
                light = g_lightArray.get( g_cLight );
                light.d_spot_cutoff = 180.0f; // No spot light
                light.d_pointLight = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        case 'd':
                light = g_lightArray.get( g_cLight );
                light.d_pointLight = true;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        // spot light on/off
        case 'S':
                light = g_lightArray.get( g_cLight );
                light.d_pointLight = true; // No directional lighting
                light.d_spot_cutoff = 90.0f;
                g_control.d_spot = true;
                g_control.d_attenuation = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                cerr << "Spot light: " <<  light.d_spot_exponent
                     << " " << light.d_spot_cutoff << endl;
                break;
        case 's':
                light = g_lightArray.get( g_cLight );
                light.d_spot_cutoff = 180.0f;
                g_control.d_spot = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                cerr << "Spot light: " <<  light.d_spot_exponent
                     << " " << light.d_spot_cutoff << endl;
                break;
        // attenuation on/off
        case 'T':
                light = g_lightArray.get( g_cLight );
                light.d_constant_attenuation = 1.0f;
                light.d_linear_attenuation = 0.001f;
                light.d_quadratic_attenuation = 0.0005f;
                g_control.d_attenuation = true;
                g_control.d_spot = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                cerr << "Attenuation: " <<  light.d_constant_attenuation
                     << " " <<light.d_linear_attenuation
                     << " " << light.d_quadratic_attenuation << endl;
                break;
        case 't':
                light = g_lightArray.get( g_cLight );
                light.d_constant_attenuation = 1.0f;
                light.d_linear_attenuation = 0.0f;
                light.d_quadratic_attenuation = 0.0f;
                g_control.d_attenuation = false;
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                cerr << "Attenuation: " <<  light.d_constant_attenuation
                     << " " << light.d_linear_attenuation
                     << " " << light.d_quadratic_attenuation << endl;
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

void specialkeys( int key, int x, int y )
{
        LightSource light;

        switch (key) {
        case GLUT_KEY_LEFT:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_exponent = std::max(light.d_spot_exponent-0.1f, 0.0f);
                        cerr << "Spot light: " <<  light.d_spot_exponent
                             << " " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_quadratic_attenuation =
                                std::max(light.d_quadratic_attenuation-0.0005f, 0.0f);
                        cerr << "Attenuation: " <<  light.d_constant_attenuation
                             << " " << light.d_linear_attenuation
                             << " " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        case GLUT_KEY_RIGHT:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_exponent =
                                std::min( light.d_spot_exponent+0.1f, 128.0f);
                        cerr << "Spot light: " <<  light.d_spot_exponent
                             << " " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_quadratic_attenuation += 0.0005f;
                        cerr << "Attenuation: " <<  light.d_constant_attenuation
                             << " " << light.d_linear_attenuation
                             << " " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        case GLUT_KEY_UP:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_cutoff = std::min(light.d_spot_cutoff+1.0f, 90.0f);
                        cerr << "Spot light: " <<  light.d_spot_exponent
                             << " " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_linear_attenuation = light.d_linear_attenuation+0.001f;
                        cerr << "Attenuation: " <<  light.d_constant_attenuation
                             << " " << light.d_linear_attenuation
                             << " " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
                break;
        case GLUT_KEY_DOWN:
                light = g_lightArray.get( g_cLight );
                if (g_control.d_spot) {
                        light.d_spot_cutoff = std::max(light.d_spot_cutoff-1.0f, 0.0f);
                        cerr << "Spot light: " <<  light.d_spot_exponent
                             << " " << light.d_spot_cutoff << endl;
                }
                if (g_control.d_attenuation) {
                        light.d_linear_attenuation =
                                std::max(light.d_linear_attenuation-0.001f, 0.0f);
                        cerr << "Attenuation: " <<  light.d_constant_attenuation
                             << " " << light.d_linear_attenuation
                             << " " << light.d_quadratic_attenuation << endl;
                }
                g_lightArray.set( g_cLight, light );
                g_lightArray.setLight(g_program, g_cLight );
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
