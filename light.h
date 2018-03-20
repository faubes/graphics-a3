// ==========================================================================
// $Id: light.h,v 1.2 2014/10/18 14:03:56 jlang Exp $
// Light example for hierachical modeling
// Modified code from solar2.cpp created by Francisco Javier Ovalle Martinez
// inspired by the OpenGL programming guide by Shreiner et al.
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
// $Log: light.h,v $
// Revision 1.2  2014/10/18 14:03:56  jlang
// Added extra checks in setLights and switched to radians.
//
// Revision 1.3  2014/03/13 23:48:43  jlang
// Added check of return value for getUniformLoc
//
// Revision 1.2  2014/03/10 04:27:22  jlang
// Check-in of debugging version
//
// Revision 1.1  2014/03/09 05:00:09  jlang
// Added viewer, light and material files
//
// Revision 1.1  2014/03/01 22:26:07  jlang
// Created lab 6 with material parameters and use of ubo
//
// Revision 1.2  2014/02/22 19:51:39  jlang
// Drawing with indexed materials and 1 light source
//
// Revision 1.1  2014/02/17 18:52:57  jlang
// Draft lab5
//
// Revision 1.1  2011/03/06 03:46:54  jlang
// Created lab 7
//
// Revision 1.2  2011/02/26 22:12:24  jlang
// Completed Lab 6
//
// Revision 1.1  2011/02/26 03:53:14  jlang
// Created skeleton for lighting lab
//
// ==========================================================================
#ifndef CSI4130_LIGHT_H
#define CSI4130_LIGHT_H

#include <cassert>
#include <sstream>
#include <string>

// gl types
#include <GL/glew.h>
// glm types
#include <glm/glm.hpp>


namespace CSI4130 {

// Use an array of uniform struct for light source
struct LightSource {
	typedef glm::vec4 vec4;
	typedef glm::vec3 vec3;

  glm::vec4 d_ambient;
  glm::vec4 d_diffuse;
  glm::vec4 d_specular;
  // switch between point and directional light source
  bool d_pointLight;
  // spot light
  // v is the vector to the vertex
  // if dir*v < cos(cutoff) then (dir * v)^N 
   glm::vec3 d_spot_direction;
  GLfloat d_spot_exponent;
  GLfloat d_spot_cutoff;
  // attentuation 1/(k_c + k_l r + k_q r^2) 
  // r is the distance of a vertex from the light source
  GLfloat d_constant_attenuation;
  GLfloat d_linear_attenuation;
  GLfloat d_quadratic_attenuation;

  glm::vec4 d_position;

  // default ctor
  LightSource() : d_ambient( 1.0f, 1.0f, 1.0f, 1.0f ), 
	  // dim (default) first light for testing
	  //d_diffuse(0.0f),
	  //d_specular(0.0f),
	d_diffuse( 1.0f, 1.0f, 1.0f, 1.0f ),
    d_specular( 1.0f, 1.0f, 1.0f, 1.0f ), 
    d_pointLight( true ),
    d_spot_direction( 0.0f, 0.0f, -1.0f ),
    d_spot_exponent(0),
    d_spot_cutoff(80.0f),
    d_constant_attenuation(1.0f),
    d_linear_attenuation(0.0f),
    d_quadratic_attenuation(0.0f),
    d_position( 0.0f, 0.0f, 0.0f, 0.0f ) {}

  // verbose ctor
  LightSource(vec4 _ambient, 
	  vec4 _diffuse, 
	  vec4 _specular, 
	  bool _pointLight, 
	  vec3 _spot_dir,
	  GLfloat _spot_exponent, 
	  GLfloat _spot_cutoff, 
	  GLfloat _a1, 
	  GLfloat _a2, 
	  GLfloat _a3, 
	  vec4 _pos ) 
	  : d_ambient(_ambient),
	  d_diffuse(_diffuse),
	  d_specular(_specular),
	  d_pointLight(_pointLight),
	  d_spot_direction(_spot_dir),
	  d_spot_exponent(_spot_exponent),
	  d_spot_cutoff(_spot_cutoff),
	  d_constant_attenuation(_a1),
	  d_linear_attenuation(_a2),
	  d_quadratic_attenuation(_a3),
	  d_position(_pos) {}
};


class LightArray {
  const static int STRIDE = 26;
  std::vector<LightSource> d_lights;
    
public:
  LightSource get( int i ) {
    assert(i < d_lights.size());
    return d_lights[i];
  }

  void set( int _i, const LightSource& _ls) {
    assert(_i < d_lights.size());
    d_lights[_i] = _ls;
    return;
  }

  void append( const LightSource& _ls) {
    d_lights.push_back(_ls);
    return;
  }

  size_t size() {
    return d_lights.size();
  }

  void setNLights(GLuint program) {
	  GLint locNLight = -1;
	  if ((locNLight = glGetUniformLocation(program, "nLights")) >= 0)
		  glProgramUniform1i(program, locNLight, static_cast<GLint>(size()));
  }

  void setLight( GLuint program, int _l ) {
    assert( _l < d_lights.size());
    GLint locLight = -1;
    std::string varName("lights[");
    std::ostringstream os;
    os << varName << _l;
    varName = os.str();
    if ((locLight = glGetUniformLocation(program, (varName + "].ambient").c_str()))>=0)
      glProgramUniform4fv(program, locLight, 1, glm::value_ptr(d_lights[_l].d_ambient));
    if ((locLight = glGetUniformLocation(program, (varName  + "].diffuse").c_str()))>=0)
      glProgramUniform4fv(program, locLight, 1, glm::value_ptr(d_lights[_l].d_diffuse));
    if ((locLight = glGetUniformLocation(program, (varName  + "].specular").c_str()))>=0)
      glProgramUniform4fv(program, locLight, 1, glm::value_ptr(d_lights[_l].d_specular));
    // boolean pointLight is not used in shader
    if ((locLight = glGetUniformLocation(program, (varName  + "].spot_direction").c_str()))>=0)
      glProgramUniform3fv(program, locLight, 1, glm::value_ptr(d_lights[_l].d_spot_direction));
    if ((locLight = glGetUniformLocation(program, (varName  + "].spot_exponent").c_str()))>=0)
      glProgramUniform1f(program, locLight, d_lights[_l].d_spot_exponent);
    if ((locLight = glGetUniformLocation(program, (varName  + "].spot_cutoff").c_str()))>=0)
      glProgramUniform1f(program, locLight, d_lights[_l].d_spot_cutoff);
    if ((locLight = glGetUniformLocation(program, (varName  + "].constant_attenuation").c_str()))>=0)
      glProgramUniform1f(program, locLight, d_lights[_l].d_constant_attenuation);
    if ((locLight = glGetUniformLocation(program, (varName  + "].linear_attenuation").c_str()))>=0)
      glProgramUniform1f(program, locLight, d_lights[_l].d_linear_attenuation);
    if ((locLight = glGetUniformLocation(program, (varName  + "].quadratic_attenuation").c_str()))>=0)
      glProgramUniform1f(program, locLight, d_lights[_l].d_quadratic_attenuation);
  
    return;
  }

  void setPosition( GLuint program, int _l ) {
    assert( _l < d_lights.size());
    std::string varName("lightPosition[");
    std::ostringstream os;
    os << varName << _l;
    varName = os.str();
    GLint locLight = glGetUniformLocation(program, (varName  + "]").c_str());
    glProgramUniform4fv(program, locLight, 1, glm::value_ptr(d_lights[_l].d_position));
    return;
  }

  void setLights( GLuint program ) {
    size_t maxLight = d_lights.size();
    for ( int l=0; l<maxLight; ++l )  {
      setLight(program, l );
    }
    return;
  }


  void setPositions( GLuint program ) {
    size_t maxLight = d_lights.size();
    std::string varName("lights[");
    for ( int l=0; l<maxLight; ++l )  {
      setPosition(program, l );
    }
    return;
  }

  // still under development?
  void setLightsUBO(GLuint _ubo) {
	  glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	  // We will send our data one material at a time
	  int lId = 0;
	  // cerr <<  "Overall size: " << STRIDE * d_materials.size() << endl;
	  for (std::vector<LightSource>::const_iterator iter = d_lights.begin();
		  iter != d_lights.end(); ++iter) {
		  LightSource l = *iter;
		  // cerr << "Offset: " << lId*STRIDE*sizeof(float) << " Size: " << sizeof(LightSource) << endl;
		  glBufferSubData(GL_UNIFORM_BUFFER, lId * STRIDE * sizeof(float), sizeof(LightSource), reinterpret_cast<const GLvoid*>(&l));
		  ++lId;
		  errorOut();
	  }
	  // glBindBuffer(GL_UNIFORM_BUFFER, 0);
	  return;
  }
};


}; // end namespace
#endif
