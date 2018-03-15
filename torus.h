// ==========================================================================
// $Id: torus.h,v 1.2 2018/02/17 03:21:13 jlang Exp $
// Simple structure for torus data
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
// Creator: jlang (Jochen Lang)
// Email:   jlang@site.uottawa.ca
// ==========================================================================
// $Log: torus.h,v $
// Revision 1.2  2018/02/17 03:21:13  jlang
// Simplified shape hierarchy for simplifying modifications
//
// Revision 1.1  2018/01/28 05:45:57  jlang
// Switch to core
//
//
// ==========================================================================
#ifndef CSI4130_TORUS_H_
#define CSI4130_TORUS_H_
#define GLM_ENABLE_EXPERIMENTAL
// gl types
#include <GL/glew.h>
// glm types
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/integer.hpp> // for modulo?
#include "render_shape.h"
#include <iostream>

class Torus : public RenderShape {
 public:
  Torus();
  void reshape(GLfloat ro, GLfloat ri, GLushort ni, GLushort no);
};
#endif
