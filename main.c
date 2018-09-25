#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ext.h"

static GLfloat triangle_vertices[] = {
  0.0, 0.8,
  -0.8, -0.8,
  0.8, -0.8,
};

static GLuint program;
static GLint attribute_coord2d;

static int init_resources();
static void onDisplay();

int main(int argc, char* argv[]) {
  ruby_init();

  Init_vismeit();

  glutInit(&argc, argv);
  glutInitContextVersion(2, 0);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("My First Triangle");

  const GLenum glew_status = glewInit();

  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }

  if (init_resources()) {
    glutDisplayFunc(onDisplay);
    glutMainLoop();
  }

  ruby_cleanup(0);
  return 0;
}

int init_resources()
{
  const VALUE rb_program = rb_eval_string(
    "Vismeit::Program.new([                                      \n"
    "  Vismeit::Shader.new(:vertex, File.read('shader.vert')),   \n"
    "  Vismeit::Shader.new(:fragment, File.read('shader.frag')), \n"
    "])                                                          \n"
  );

  rb_mVismeit_cProgram_CDATA *program_cdata;
  Data_Get_Struct(rb_program, rb_mVismeit_cProgram_CDATA, program_cdata);

  program = program_cdata->gl_id;

  const char *const attribute_name = "coord2d";
  attribute_coord2d = glGetAttribLocation(program, attribute_name);

  if (attribute_coord2d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  return 1;
}

void onDisplay()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);
  glEnableVertexAttribArray(attribute_coord2d);

  glVertexAttribPointer(
    attribute_coord2d,
    2,
    GL_FLOAT,
    GL_FALSE,
    0,
    triangle_vertices
  );

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(attribute_coord2d);
  glutSwapBuffers();
}
