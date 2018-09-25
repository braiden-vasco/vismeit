#ifndef __VISMEIT_H__
#define __VISMEIT_H__

#include <ruby.h>
#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GLuint gl_id;
} rb_mVismeit_cShader_CDATA;

typedef struct {
  GLuint gl_id;
} rb_mVismeit_cProgram_CDATA;

typedef struct {
  GLint gl_id;
} rb_mVismeit_cAttrib_CDATA;

extern void Init_vismeit();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __VISMEIT_H__
