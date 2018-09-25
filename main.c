#include <stdio.h>
#include <stdlib.h>
#include <ruby.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

typedef struct {
  GLuint gl_id;
} rb_mVismeit_cShader_CDATA;

static void Init_vismeit();

static VALUE rb_mVismeit;
static VALUE rb_mVismeit_cShader;

static VALUE rb_mVismeit_cShader_alloc(VALUE rb_klass);
static void  rb_mVismeit_cShader_free(rb_mVismeit_cShader_CDATA *free_cdata);

static VALUE rb_mVismeit_cShader_initialize(VALUE rb_self,
                                            VALUE rb_type, VALUE rb_source);

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

static GLfloat triangle_vertices[] = {
  0.0, 0.8,
  -0.8, -0.8,
  0.8, -0.8,
};

static GLuint program;
static GLint attribute_coord2d;

static int init_resources();
static void onDisplay();
static void free_resources();

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

  free_resources();
  ruby_cleanup(0);
  return 0;
}

int init_resources()
{
  const VALUE vertex_shader = rb_eval_string(
    "Vismeit::Shader.new(                                 \n"
    "  :vertex,                                           \n"
    "  \"#version 120                             \\n\" \\\n"
    "  \"attribute vec2 coord2d;                  \\n\" \\\n"
    "  \"void main(void) {                        \\n\" \\\n"
    "  \"  gl_Position = vec4(coord2d, 0.0, 1.0); \\n\" \\\n"
    "  \"}                                        \\n\",  \n"
    ")                                                    \n"
  );

  const VALUE fragment_shader = rb_eval_string(
    "Vismeit::Shader.new(                                 \n"
    "  :fragment,                                         \n"
    "  \"#version 120                             \\n\" \\\n"
    "  \"void main(void) {                        \\n\" \\\n"
    "  \"  gl_FragColor[0] = 0.0;                 \\n\" \\\n"
    "  \"  gl_FragColor[1] = 0.0;                 \\n\" \\\n"
    "  \"  gl_FragColor[2] = 1.0;                 \\n\" \\\n"
    "  \"}                                        \\n\" \\\n"
    ")                                                    \n"
  );

  GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

  const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vs_source, NULL);
  glCompileShader(vs);

  GLint vertex_shader_log_length;
  glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &vertex_shader_log_length);

  if (vertex_shader_log_length > 0) {
    char vertex_shader_log[vertex_shader_log_length];
    glGetShaderInfoLog(vs, vertex_shader_log_length, NULL, vertex_shader_log);
    fprintf(stderr, vertex_shader_log);
  }

  glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);

  if (!compile_ok) {
    fprintf(stderr, "Error in vertex shader\n");
    return 0;
  }

  const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fs_source, NULL);
  glCompileShader(fs);

  GLint fragment_shader_log_length;
  glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &fragment_shader_log_length);

  if (fragment_shader_log_length > 0) {
    char fragment_shader_log[fragment_shader_log_length];
    glGetShaderInfoLog(fs, fragment_shader_log_length, NULL, fragment_shader_log);
    fprintf(stderr, fragment_shader_log);
  }

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

  const char *const attribute_name = "coord2d";
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

void Init_vismeit()
{
  rb_mVismeit = rb_define_module("Vismeit");

  rb_mVismeit_cShader =
    rb_define_class_under(rb_mVismeit, "Shader", rb_cObject);

  rb_define_alloc_func(rb_mVismeit_cShader, rb_mVismeit_cShader_alloc);

  rb_define_method(rb_mVismeit_cShader, "initialize",
                   rb_mVismeit_cShader_initialize, 2);
}

VALUE rb_mVismeit_cShader_alloc(const VALUE rb_klass)
{
  rb_mVismeit_cShader_CDATA *const alloc_cdata =
    ALLOC(rb_mVismeit_cShader_CDATA);

  memset(alloc_cdata, 0, sizeof(rb_mVismeit_cShader_CDATA));

  const VALUE result = Data_Wrap_Struct(rb_klass, NULL,
                                        rb_mVismeit_cShader_free, alloc_cdata);

  return result;
}

void rb_mVismeit_cShader_free(rb_mVismeit_cShader_CDATA *const free_cdata)
{
  glDeleteShader(free_cdata->gl_id);
  free(free_cdata);
}

VALUE rb_mVismeit_cShader_initialize(
  const VALUE rb_self,
  const VALUE rb_type,
  const VALUE rb_source
)
{
  Check_Type(rb_source, T_STRING);

  GLenum gl_shader_type;

  if (rb_funcall(rb_type, rb_intern("=="), 1, ID2SYM(rb_intern("vertex"))))
  {
    gl_shader_type = GL_VERTEX_SHADER;
  }
  else if (rb_funcall(rb_type, rb_intern("=="), 1,
                      ID2SYM(rb_intern("fragment"))))
  {
    gl_shader_type = GL_FRAGMENT_SHADER;
  }
  else
  {
    rb_raise(rb_eRuntimeError, "invalid type");
  }

  rb_mVismeit_cShader_CDATA *self_cdata;
  Data_Get_Struct(rb_self, rb_mVismeit_cShader_CDATA, self_cdata);

  self_cdata->gl_id = glCreateShader(gl_shader_type);

  if (self_cdata->gl_id == 0)
  {
    rb_raise(rb_eRuntimeError, "can not generate shader object");
  }

  const GLchar *const gl_source_text = RSTRING_PTR(rb_source);
  const GLint const gl_source_length = RSTRING_LEN(rb_source);

  glShaderSource(self_cdata->gl_id, 1, &gl_source_text, &gl_source_length);

  glCompileShader(self_cdata->gl_id);

  GLint gl_compile_status;
  glGetShaderiv(self_cdata->gl_id, GL_COMPILE_STATUS, &gl_compile_status);

  if (gl_compile_status == 0)
  {
    rb_raise(rb_eRuntimeError, "can not compile shader");
  }

  return rb_self;
}
