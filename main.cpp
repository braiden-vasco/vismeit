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

static int init_resources();

static void on_display();
static void on_reshape(int width, int height);

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

    glutMainLoop();
  }

  ruby_cleanup(0);
  return 0;
}

int init_resources()
{
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

  glutSwapBuffers();
}

void on_reshape(const int width, const int height)
{
  screen_width = width;
  screen_height = height;
}
