#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ext.h"

static const char *const window_title = "Vismeit";

static const int initial_window_width  = 640;
static const int initial_window_height = 480;

static int screen_width = initial_window_width;
static int screen_height = initial_window_height;

struct Vertex3fAttribute
{
  GLfloat x, y, z;
};

struct Color3fAttribute
{
  GLfloat r, g, b;
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
static GLint uniform_mvp;
static GLuint vbo_cube_vertices, vbo_cube_colors;
static GLuint ibo_cube_elements;

static int init_resources();

static void on_display();
static void on_reshape(int width, int height);
static void on_idle();

int main(int argc, char* argv[]) {
  ruby_init();

  Init_vismeit();

  glutInit(&argc, argv);
  glutInitContextVersion(2, 0);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize(initial_window_width, initial_window_height);
  glutCreateWindow(window_title);

  const GLenum glew_status = glewInit();

  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }

  if (init_resources()) {
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutIdleFunc(on_idle);

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

  const VALUE rb_coord3d_attrib = rb_funcall(
    rb_eval_string("Vismeit::Attrib"),
    rb_intern("new"),
    2,
    rb_program,
    rb_str_new_cstr("coord3d")
  );

  const VALUE rb_v_color_attrib = rb_funcall(
    rb_eval_string("Vismeit::Attrib"),
    rb_intern("new"),
    2,
    rb_program,
    rb_str_new_cstr("v_color")
  );

  rb_mVismeit_cProgram_CDATA *cdata_program;
  rb_mVismeit_cAttrib_CDATA  *cdata_coord3d_attrib;
  rb_mVismeit_cAttrib_CDATA  *cdata_v_color_attrib;

  Data_Get_Struct(rb_program,        rb_mVismeit_cProgram_CDATA, cdata_program);
  Data_Get_Struct(rb_coord3d_attrib, rb_mVismeit_cAttrib_CDATA,  cdata_coord3d_attrib);
  Data_Get_Struct(rb_v_color_attrib, rb_mVismeit_cAttrib_CDATA,  cdata_v_color_attrib);

  program           = cdata_program->gl_id;
  attribute_coord3d = cdata_coord3d_attrib->gl_id;
  attribute_v_color = cdata_v_color_attrib->gl_id;

  uniform_mvp = glGetUniformLocation(program, "mvp");

  if (uniform_mvp == -1) {
    fprintf(stderr, "Could not bind unform %s\n", "mvp");
    return 0;
  }

  glGenBuffers(1, &vbo_cube_vertices);
  glGenBuffers(1, &vbo_cube_colors);
  glGenBuffers(1, &ibo_cube_elements);

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

void on_display()
{
  glViewport(0, 0, screen_width, screen_height);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(1.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glUseProgram(program);

  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_v_color);

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

void on_reshape(const int width, const int height)
{
  screen_width = width;
  screen_height = height;
}

void on_idle()
{
  const float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
  glm::vec3 axis_y(0, 1, 0);
  glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 10.0f);

  glm::mat4 mvp = projection * view * model * anim;

  glUseProgram(program);
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glutPostRedisplay();
}
