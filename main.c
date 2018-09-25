#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

static const char *const vs_source =
#ifdef GL_ES_VERSION_2_0
  "#version 100                             \n"  // OpenGL ES 2.0
#else
  "#version 120                             \n"  // OpenGL 2.1
#endif
  "attribute vec2 coord2d;                  \n"
  "void main(void) {                        \n"
  "  gl_Position = vec4(coord2d, 0.0, 1.0); \n"
  "}                                        \n"
;

static const char *const fs_source =
#ifdef GL_ES_VERSION_2_0
  "#version 100             \n"  // OpenGL ES 2.0
#else
  "#version 120             \n"  // OpenGL 2.1
#endif
  "void main(void) {        \n"
  "  gl_FragColor[0] = 0.0; \n"
  "  gl_FragColor[1] = 0.0; \n"
  "  gl_FragColor[2] = 1.0; \n"
  "}                        \n"
;

static GLuint program;
static GLint attribute_coord2d;

static int init_resources();
static void onDisplay();
static void free_resources();

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(2,0);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("My First Triangle");

  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }

  if (init_resources()) {
    glutDisplayFunc(onDisplay);
    glutMainLoop();
  }

  free_resources();
  return 0;
}

int init_resources()
{
  GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vs_source, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    fprintf(stderr, "Error in vertex shader\n");
    return 0;
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fs_source, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    fprintf(stderr, "Error in fragment shader\n");
    return 0;
  }

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    return 0;
  }

  const char* attribute_name = "coord2d";
  attribute_coord2d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord2d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  return 1;
}

void free_resources()
{
  glDeleteProgram(program);
}

void onDisplay()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);
  glEnableVertexAttribArray(attribute_coord2d);
  GLfloat triangle_vertices[] = {
     0.0,  0.8,
    -0.8, -0.8,
     0.8, -0.8,
  };
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
