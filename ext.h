#ifndef __VISMEIT_H__
#define __VISMEIT_H__

#include <ruby.h>
#include <GL/glew.h>

typedef struct {
  GLuint gl_id;
} rb_mVismeit_cShader_CDATA;

void Init_vismeit();

#endif // __VISMEIT_H__
