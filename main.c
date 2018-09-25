#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ext.h"

static GLfloat triangle_data[] = {
   0.0,  0.8, 0.0,  1.0, 1.0, 0.0,
  -0.8, -0.8, 0.0,  0.0, 0.0, 1.0,
   0.8, -0.8, 0.0,  1.0, 0.0, 0.0,
};

static GLuint program;
static GLint attribute_coord2d, attribute_v_color;
static GLuint vbo_triangle;

static int init_resources();
static void free_resources();

static void render();

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
    glutDisplayFunc(render);
    glutMainLoop();
  }

  free_resources();
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

  const char *const attribute_v_color_name = "v_color";
  attribute_v_color = glGetAttribLocation(program, attribute_v_color_name);

  if (attribute_v_color == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_v_color_name);
    return 0;
  }

  glGenBuffers(1, &vbo_triangle);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(triangle_data),
    triangle_data,
    GL_STATIC_DRAW
  );

  return 1;
}

void free_resources()
{
  glDeleteBuffers(1, &vbo_triangle);
}

void render()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);

  glEnableVertexAttribArray(attribute_coord2d);
  glEnableVertexAttribArray(attribute_v_color);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glVertexAttribPointer(
    attribute_coord2d,
    2,
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(GL_FLOAT),
    0
  );

  glVertexAttribPointer(
    attribute_v_color,
    3,
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(GL_FLOAT),
    (GLvoid*)(3 * sizeof(GL_FLOAT))
  );

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(attribute_coord2d);
  glutSwapBuffers();
}
