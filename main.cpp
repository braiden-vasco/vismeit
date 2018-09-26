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

static GLuint a_program, b_program;
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
  const VALUE rb_cube_vertex_attributes = rb_eval_string(
    "["
    "  -1.0, -1.0,  1.0,"
    "   1.0, -1.0,  1.0,"
    "   1.0,  1.0,  1.0,"
    "  -1.0,  1.0,  1.0,"

    "  -1.0, -1.0, -1.0,"
    "   1.0, -1.0, -1.0,"
    "   1.0,  1.0, -1.0,"
    "  -1.0,  1.0, -1.0,"
    "].pack('f*').freeze" // float
  );

  const VALUE rb_cube_color_attributes = rb_eval_string(
    "["
    "  1.0, 0.0, 0.0,"
    "  0.0, 1.0, 0.0,"
    "  0.0, 0.0, 1.0,"
    "  1.0, 1.0, 1.0,"

    "  1.0, 0.0, 0.0,"
    "  0.0, 1.0, 0.0,"
    "  0.0, 0.0, 1.0,"
    "  1.0, 1.0, 1.0,"
    "].pack('f*').freeze" // float
  );

  const VALUE rb_cube_elements = rb_eval_string(
    "["
    "  0, 1, 2,"
    "  2, 3, 0,"

    "  1, 5, 6,"
    "  6, 2, 1,"

    "  7, 6, 5,"
    "  5, 4, 7,"

    "  4, 0, 3,"
    "  3, 7, 4,"

    "  4, 5, 1,"
    "  1, 0, 4,"

    "  3, 2, 6,"
    "  6, 7, 3,"
    "].pack('S*').freeze" // unsigned short
  );

  const VALUE rb_a_program = rb_eval_string(
    "Vismeit::Program.new([                                           \n"
    "  Vismeit::Shader.new(:vertex_shader,   File.read('sh/a.vert')), \n"
    "  Vismeit::Shader.new(:fragment_shader, File.read('sh/a.frag')), \n"
    "])                                                               \n"
  );

  const VALUE rb_b_program = rb_eval_string(
    "Vismeit::Program.new([                                           \n"
    "  Vismeit::Shader.new(:vertex_shader,   File.read('sh/b.vert')), \n"
    "  Vismeit::Shader.new(:fragment_shader, File.read('sh/b.frag')), \n"
    "])                                                               \n"
  );

  const VALUE rb_coord3d_attrib = rb_funcall(
    rb_eval_string("Vismeit::Attrib"),
    rb_intern("new"),
    2,
    rb_a_program,
    rb_str_new_cstr("coord3d")
  );

  const VALUE rb_v_color_attrib = rb_funcall(
    rb_eval_string("Vismeit::Attrib"),
    rb_intern("new"),
    2,
    rb_a_program,
    rb_str_new_cstr("v_color")
  );

  const VALUE rb_mvp_uniform = rb_funcall(
    rb_eval_string("Vismeit::Uniform"),
    rb_intern("new"),
    2,
    rb_a_program,
    rb_str_new_cstr("mvp")
  );

  const VALUE rb_cube_vertex_vbo = rb_funcall(
    rb_eval_string("Vismeit::ArrayBuffer"),
    rb_intern("new"),
    1,
    rb_cube_vertex_attributes
  );

  const VALUE rb_cube_color_vbo = rb_funcall(
    rb_eval_string("Vismeit::ArrayBuffer"),
    rb_intern("new"),
    1,
    rb_cube_color_attributes
  );

  const VALUE rb_cube_element_ibo = rb_funcall(
    rb_eval_string("Vismeit::ElementArrayBuffer"),
    rb_intern("new"),
    1,
    rb_cube_elements
  );

  CDATA_mVismeit_cProgram            *cdata_a_program;
  CDATA_mVismeit_cProgram            *cdata_b_program;
  CDATA_mVismeit_cAttrib             *cdata_coord3d_attrib;
  CDATA_mVismeit_cAttrib             *cdata_v_color_attrib;
  CDATA_mVismeit_cUniform            *cdata_mvp_uniform;
  CDATA_mVismeit_cArrayBuffer        *cdata_cube_vertex_vbo;
  CDATA_mVismeit_cArrayBuffer        *cdata_cube_color_vbo;
  CDATA_mVismeit_cElementArrayBuffer *cdata_cube_element_ibo;

  Data_Get_Struct(rb_a_program,
                  CDATA_mVismeit_cProgram,            cdata_a_program);
  Data_Get_Struct(rb_b_program,
                  CDATA_mVismeit_cProgram,            cdata_b_program);
  Data_Get_Struct(rb_coord3d_attrib,
                  CDATA_mVismeit_cAttrib,             cdata_coord3d_attrib);
  Data_Get_Struct(rb_v_color_attrib,
                  CDATA_mVismeit_cAttrib,             cdata_v_color_attrib);
  Data_Get_Struct(rb_mvp_uniform,
                  CDATA_mVismeit_cUniform,            cdata_mvp_uniform);
  Data_Get_Struct(rb_cube_vertex_vbo,
                  CDATA_mVismeit_cArrayBuffer,        cdata_cube_vertex_vbo);
  Data_Get_Struct(rb_cube_color_vbo,
                  CDATA_mVismeit_cArrayBuffer,        cdata_cube_color_vbo);
  Data_Get_Struct(rb_cube_element_ibo,
                  CDATA_mVismeit_cElementArrayBuffer, cdata_cube_element_ibo);

  a_program         = cdata_a_program->gl_id;
  b_program         = cdata_b_program->gl_id;
  attribute_coord3d = cdata_coord3d_attrib->gl_id;
  attribute_v_color = cdata_v_color_attrib->gl_id;
  uniform_mvp       = cdata_mvp_uniform->gl_id;
  vbo_cube_vertices = cdata_cube_vertex_vbo->gl_id;
  vbo_cube_colors   = cdata_cube_color_vbo->gl_id;
  ibo_cube_elements = cdata_cube_element_ibo->gl_id;

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

  glUseProgram(a_program);

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

  glUseProgram(a_program);
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glutPostRedisplay();
}
