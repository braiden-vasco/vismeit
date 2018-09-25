#ifndef __VISMEIT_H__
#define __VISMEIT_H__

#include <ruby.h>
#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GLuint gl_id;
} CDATA_mVismeit_cShader;

typedef struct {
  GLuint gl_id;
} CDATA_mVismeit_cProgram;

typedef struct {
  GLint gl_id;
} CDATA_mVismeit_cAttrib;

extern void Init_vismeit();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __VISMEIT_H__
