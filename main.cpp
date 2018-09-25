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

static const struct Vertex3fColor3fAttribute triangle_attributes[] = {
  {{ 0.0,  0.8, 0.0}, {1.0, 1.0, 0.0}},
  {{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
  {{ 0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}},
};

static const struct Vertex3fAttribute cube_vertex_attributes[] = {
  {-1.0, -1.0,  1.0},
  { 1.0, -1.0,  1.0},
  { 1.0,  1.0,  1.0},
  {-1.0,  1.0,  1.0},

  {-1.0, -1.0, -1.0},
  { 1.0, -1.0, -1.0},
  { 1.0,  1.0, -1.0},
  {-1.0,  1.0, -1.0},
};

static const struct Color3fAttribute cube_color_attributes[] = {
  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 1.0, 1.0},

  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 1.0, 1.0},
};

static const GLushort cube_elements[] = {
  // front
  0, 1, 2,
  2, 3, 0,
  // right
  1, 5, 6,
  6, 2, 1,
  // back
  7, 6, 5,
  5, 4, 7,
  // left
  4, 0, 3,
  3, 7, 4,
  // bottom
  4, 5, 1,
  1, 0, 4,
  // top
  3, 2, 6,
  6, 7, 3,
};

static GLuint program;
static GLint attribute_coord3d, attribute_v_color;
static GLint uniform_m_transform;
static GLuint vbo_triangle, vbo_cube_vertices, vbo_cube_colors;
static GLuint ibo_cube_elements;

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

  attribute_coord3d = glGetAttribLocation(program, "coord3d");

  if (attribute_coord3d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", "coord3d");
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
  glGenBuffers(1, &vbo_cube_vertices);
  glGenBuffers(1, &vbo_cube_colors);
  glGenBuffers(1, &ibo_cube_elements);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(triangle_attributes),
    triangle_attributes,
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(cube_vertex_attributes),
    cube_vertex_attributes,
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(cube_color_attributes),
    cube_color_attributes,
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(cube_elements),
    cube_elements,
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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(1.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  glm::mat4 m_transform =
    glm::rotate(glm::mat4(1.0), 0.25f, glm::vec3(0.0, 0.0, 1.0));

  glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));

  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_v_color);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

  glVertexAttribPointer(
    attribute_coord3d,
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

  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

  glVertexAttribPointer(
    attribute_coord3d,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0
  );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);

  glVertexAttribPointer(
    attribute_v_color,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);

  int size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_v_color);

  glutSwapBuffers();
}
