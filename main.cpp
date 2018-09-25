#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ext.h"

struct Vertex3fAttribute
{
  GLfloat x, y, z;
};

struct Color3fAttribute
{
  GLfloat r, g, b;
};

struct Vertex3fColor3fAttribute
{
  struct Vertex3fAttribute vertex;
  struct Color3fAttribute color;
};

static struct Vertex3fColor3fAttribute triangle_attributes[] = {
  {{ 0.0,  0.8, 0.0}, {1.0, 1.0, 0.0}},
  {{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
  {{ 0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}},
};

static GLuint program;
static GLint attribute_coord2d, attribute_v_color;
static GLint uniform_m_transform;
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

  attribute_coord2d = glGetAttribLocation(program, "coord2d");

  if (attribute_coord2d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", "coord2d");
    return 0;
  }

  attribute_v_color = glGetAttribLocation(program, "v_color");

  if (attribute_v_color == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", "v_color");
    return 0;
  }

  uniform_m_transform = glGetUniformLocation(program, "m_transform");

  if (uniform_m_transform == -1) {
    fprintf(stderr, "Could not bind unform %s\n", "m_transform");
    return 0;
  }

  glGenBuffers(1, &vbo_triangle);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(triangle_attributes),
    triangle_attributes,
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

  glm::mat4 m_transform =
    glm::rotate(glm::mat4(1.0), 0.25f, glm::vec3(0.0, 0.0, 1.0));

  glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));

  glEnableVertexAttribArray(attribute_coord2d);
  glEnableVertexAttribArray(attribute_v_color);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glVertexAttribPointer(
    attribute_coord2d,
    2,
    GL_FLOAT,
    GL_FALSE,
    sizeof(struct Vertex3fColor3fAttribute),
    0
  );

  glVertexAttribPointer(
    attribute_v_color,
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(struct Vertex3fColor3fAttribute),
    (GLvoid*)(sizeof(struct Vertex3fAttribute))
  );

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(attribute_coord2d);
  glutSwapBuffers();
}
