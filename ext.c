#include "ext.h"

static VALUE rb_mVismeit;
static VALUE rb_mVismeit_cShader;
static VALUE rb_mVismeit_cProgram;
static VALUE rb_mVismeit_cAttrib;
static VALUE rb_mVismeit_cUniform;
static VALUE rb_mVismeit_cArrayBuffer;
static VALUE rb_mVismeit_cElementArrayBuffer;

static VALUE rb_mVismeit_cShader_alloc(VALUE rb_klass);
static VALUE rb_mVismeit_cProgram_alloc(VALUE rb_klass);
static VALUE rb_mVismeit_cAttrib_alloc(VALUE rb_klass);
static VALUE rb_mVismeit_cUniform_alloc(VALUE rb_klass);
static VALUE rb_mVismeit_cArrayBuffer_alloc(VALUE rb_klass);
static VALUE rb_mVismeit_cElementArrayBuffer_alloc(VALUE rb_klass);

static void rb_mVismeit_cShader_free(CDATA_mVismeit_cShader   *cdata_free);
static void rb_mVismeit_cProgram_free(CDATA_mVismeit_cProgram *cdata_free);
static void rb_mVismeit_cAttrib_free(CDATA_mVismeit_cAttrib   *cdata_free);
static void rb_mVismeit_cUniform_free(CDATA_mVismeit_cUniform *cdata_free);

static void rb_mVismeit_cArrayBuffer_free(CDATA_mVismeit_cArrayBuffer
                                          *cdata_free);

static void rb_mVismeit_cElementArrayBuffer_free(
  CDATA_mVismeit_cElementArrayBuffer *cdata_free);

static VALUE rb_mVismeit_cShader_initialize(VALUE rb_self,
                                            VALUE rb_type, VALUE rb_source);

static VALUE rb_mVismeit_cProgram_initialize(VALUE rb_self, VALUE rb_shaders);

static VALUE rb_mVismeit_cAttrib_initialize(VALUE rb_self,
                                            VALUE rb_program, VALUE rb_name);

static VALUE rb_mVismeit_cUniform_initialize(VALUE rb_self,
                                             VALUE rb_program, VALUE rb_name);

static VALUE rb_mVismeit_cArrayBuffer_initialize(VALUE rb_self,
                                                 VALUE rb_data);

static VALUE rb_mVismeit_cElementArrayBuffer_initialize(VALUE rb_self,
                                                        VALUE rb_data);

void Init_vismeit()
{
  rb_mVismeit = rb_define_module("Vismeit");

  rb_mVismeit_cShader =
    rb_define_class_under(rb_mVismeit, "Shader", rb_cObject);

  rb_mVismeit_cProgram =
    rb_define_class_under(rb_mVismeit, "Program", rb_cObject);

  rb_mVismeit_cAttrib =
    rb_define_class_under(rb_mVismeit, "Attrib", rb_cObject);

  rb_mVismeit_cUniform =
    rb_define_class_under(rb_mVismeit, "Uniform", rb_cObject);

  rb_mVismeit_cArrayBuffer =
    rb_define_class_under(rb_mVismeit, "ArrayBuffer", rb_cObject);

  rb_mVismeit_cElementArrayBuffer =
    rb_define_class_under(rb_mVismeit, "ElementArrayBuffer", rb_cObject);

  rb_define_alloc_func(rb_mVismeit_cShader,  rb_mVismeit_cShader_alloc);
  rb_define_alloc_func(rb_mVismeit_cProgram, rb_mVismeit_cProgram_alloc);
  rb_define_alloc_func(rb_mVismeit_cAttrib,  rb_mVismeit_cAttrib_alloc);
  rb_define_alloc_func(rb_mVismeit_cUniform, rb_mVismeit_cUniform_alloc);

  rb_define_alloc_func(rb_mVismeit_cArrayBuffer,
                       rb_mVismeit_cArrayBuffer_alloc);

  rb_define_alloc_func(rb_mVismeit_cElementArrayBuffer,
                       rb_mVismeit_cElementArrayBuffer_alloc);

  rb_define_method(rb_mVismeit_cShader, "initialize",
                   rb_mVismeit_cShader_initialize, 2);

  rb_define_method(rb_mVismeit_cProgram, "initialize",
                   rb_mVismeit_cProgram_initialize, 1);

  rb_define_method(rb_mVismeit_cAttrib, "initialize",
                   rb_mVismeit_cAttrib_initialize, 2);

  rb_define_method(rb_mVismeit_cUniform, "initialize",
                   rb_mVismeit_cUniform_initialize, 2);

  rb_define_method(rb_mVismeit_cArrayBuffer, "initialize",
                   rb_mVismeit_cArrayBuffer_initialize, 1);

  rb_define_method(rb_mVismeit_cElementArrayBuffer, "initialize",
                   rb_mVismeit_cElementArrayBuffer_initialize, 1);
}

VALUE rb_mVismeit_cShader_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cShader *const cdata_alloc = ALLOC(CDATA_mVismeit_cShader);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cShader));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cShader_free, cdata_alloc);
}

VALUE rb_mVismeit_cProgram_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cProgram *const cdata_alloc = ALLOC(CDATA_mVismeit_cProgram);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cProgram));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cProgram_free, cdata_alloc);
}

VALUE rb_mVismeit_cAttrib_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cAttrib *const cdata_alloc = ALLOC(CDATA_mVismeit_cAttrib);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cAttrib));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cAttrib_free, cdata_alloc);
}

VALUE rb_mVismeit_cUniform_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cUniform *const cdata_alloc = ALLOC(CDATA_mVismeit_cUniform);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cUniform));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cUniform_free, cdata_alloc);
}

VALUE rb_mVismeit_cArrayBuffer_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cArrayBuffer *const cdata_alloc =
    ALLOC(CDATA_mVismeit_cArrayBuffer);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cArrayBuffer));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cArrayBuffer_free, cdata_alloc);
}

VALUE rb_mVismeit_cElementArrayBuffer_alloc(const VALUE rb_klass)
{
  CDATA_mVismeit_cElementArrayBuffer *const cdata_alloc =
    ALLOC(CDATA_mVismeit_cElementArrayBuffer);

  memset(cdata_alloc, 0, sizeof(CDATA_mVismeit_cElementArrayBuffer));

  return Data_Wrap_Struct(rb_klass, NULL,
                          rb_mVismeit_cElementArrayBuffer_free, cdata_alloc);
}

void rb_mVismeit_cShader_free(CDATA_mVismeit_cShader *const cdata_free)
{
  glDeleteShader(cdata_free->gl_id);
  free(cdata_free);
}

void rb_mVismeit_cProgram_free(CDATA_mVismeit_cProgram *const cdata_free)
{
  glDeleteProgram(cdata_free->gl_id);
  free(cdata_free);
}

void rb_mVismeit_cAttrib_free(CDATA_mVismeit_cAttrib *const cdata_free)
{
  free(cdata_free);
}

void rb_mVismeit_cUniform_free(CDATA_mVismeit_cUniform *const cdata_free)
{
  free(cdata_free);
}

void rb_mVismeit_cArrayBuffer_free(CDATA_mVismeit_cArrayBuffer
                                   *const cdata_free)
{
  glDeleteBuffers(1, &cdata_free->gl_id);
  free(cdata_free);
}

void rb_mVismeit_cElementArrayBuffer_free(CDATA_mVismeit_cElementArrayBuffer
                                          *const cdata_free)
{
  glDeleteBuffers(1, &cdata_free->gl_id);
  free(cdata_free);
}

VALUE rb_mVismeit_cShader_initialize(
  const VALUE rb_self,
  const VALUE rb_type,
  const VALUE rb_source
)
{
  Check_Type(rb_type,   T_SYMBOL);
  Check_Type(rb_source, T_STRING);

  GLenum gl_shader_type;

  if (rb_funcall(rb_type, rb_intern("=="), 1,
                 ID2SYM(rb_intern("vertex_shader"))))
  {
    gl_shader_type = GL_VERTEX_SHADER;
  }
  else if (rb_funcall(rb_type, rb_intern("=="), 1,
                      ID2SYM(rb_intern("fragment_shader"))))
  {
    gl_shader_type = GL_FRAGMENT_SHADER;
  }
  else
  {
    rb_raise(rb_eRuntimeError, "invalid type");
  }

  CDATA_mVismeit_cShader *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cShader, cdata_self);

  cdata_self->gl_id = glCreateShader(gl_shader_type);

  if (cdata_self->gl_id == 0)
  {
    rb_raise(rb_eRuntimeError, "can not generate shader object");
  }

  const GLchar *const gl_source_text = RSTRING_PTR(rb_source);
  const GLint const gl_source_length = RSTRING_LEN(rb_source);

  glShaderSource(cdata_self->gl_id, 1, &gl_source_text, &gl_source_length);

  glCompileShader(cdata_self->gl_id);

  GLint gl_compile_status;
  glGetShaderiv(cdata_self->gl_id, GL_COMPILE_STATUS, &gl_compile_status);

  if (gl_compile_status == 0)
  {
    rb_raise(rb_eRuntimeError, "can not compile shader");
  }

  return rb_self;
}

VALUE rb_mVismeit_cProgram_initialize(
  const VALUE rb_self,
  const VALUE rb_shaders
)
{
  Check_Type(rb_shaders, T_ARRAY);

  CDATA_mVismeit_cProgram *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cProgram, cdata_self);

  cdata_self->gl_id = glCreateProgram();

  if (cdata_self->gl_id == 0)
  {
    rb_raise(rb_eRuntimeError, "can not generate program object");
  }

  const VALUE rb_shaders_count = rb_funcall(rb_shaders, rb_intern("count"), 0);

  const long shaders_count = NUM2LONG(rb_shaders_count);

  const VALUE rb_ivar_shaders = rb_ary_new_capa(shaders_count);

  rb_ivar_set(rb_self, rb_intern("@shaders"), rb_ivar_shaders);

  for (long i = 0; i < shaders_count; ++i)
  {
    const VALUE rb_shader = rb_ary_entry(rb_shaders, i);

    if (!rb_funcall(rb_shader, rb_intern("is_a?"), 1, rb_mVismeit_cShader))
    {
      rb_raise(rb_eTypeError, "expected ::Vismeit::Shader");
    }

    rb_ary_push(rb_ivar_shaders, rb_shader);

    CDATA_mVismeit_cShader *cdata_shader;
    Data_Get_Struct(rb_shader, CDATA_mVismeit_cShader, cdata_shader);

    glAttachShader(cdata_self->gl_id, cdata_shader->gl_id);
  }

  glLinkProgram(cdata_self->gl_id);

  GLint gl_link_status;
  glGetProgramiv(cdata_self->gl_id, GL_LINK_STATUS, &gl_link_status);

  if (gl_link_status == 0)
  {
    rb_raise(rb_eRuntimeError, "can not link program");
  }

  return rb_self;
}

VALUE rb_mVismeit_cAttrib_initialize(
  const VALUE rb_self,
  const VALUE rb_program,
  VALUE rb_name
)
{
  Check_Type(rb_name, T_STRING);

  if (!rb_funcall(rb_program, rb_intern("is_a?"), 1, rb_mVismeit_cProgram))
  {
    rb_raise(rb_eTypeError, "expected ::Vismeit::Program");
  }

  rb_ivar_set(rb_self, rb_intern("@program"), rb_program);

  CDATA_mVismeit_cAttrib *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cAttrib, cdata_self);

  CDATA_mVismeit_cProgram *cdata_program;
  Data_Get_Struct(rb_program, CDATA_mVismeit_cProgram, cdata_program);

  const char *const name = StringValueCStr(rb_name);

  cdata_self->gl_id = glGetAttribLocation(cdata_program->gl_id, name);

  if (cdata_self->gl_id == -1)
  {
    rb_raise(rb_eRuntimeError, "can not get attribute location");
  }

  return rb_self;
}

VALUE rb_mVismeit_cUniform_initialize(
  const VALUE rb_self,
  const VALUE rb_program,
  VALUE rb_name
)
{
  Check_Type(rb_name, T_STRING);

  if (!rb_funcall(rb_program, rb_intern("is_a?"), 1, rb_mVismeit_cProgram))
  {
    rb_raise(rb_eTypeError, "expected ::Vismeit::Program");
  }

  rb_ivar_set(rb_self, rb_intern("@program"), rb_program);

  CDATA_mVismeit_cUniform *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cUniform, cdata_self);

  CDATA_mVismeit_cProgram *cdata_program;
  Data_Get_Struct(rb_program, CDATA_mVismeit_cProgram, cdata_program);

  const char *const name = StringValueCStr(rb_name);

  cdata_self->gl_id = glGetUniformLocation(cdata_program->gl_id, name);

  if (cdata_self->gl_id == -1)
  {
    rb_raise(rb_eRuntimeError, "can not get uniform location");
  }

  return rb_self;
}

VALUE rb_mVismeit_cArrayBuffer_initialize(
  const VALUE rb_self,
  const VALUE rb_data
)
{
  Check_Type(rb_data, T_STRING);

  CDATA_mVismeit_cArrayBuffer *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cArrayBuffer, cdata_self);

  glGenBuffers(1, &cdata_self->gl_id);

  if (cdata_self->gl_id == 0)
  {
    rb_raise(rb_eRuntimeError, "can not generate buffer object");
  }

  glBindBuffer(GL_ARRAY_BUFFER, cdata_self->gl_id);

  glBufferData(
    GL_ARRAY_BUFFER,
    RSTRING_LEN(rb_data),
    RSTRING_PTR(rb_data),
    GL_STATIC_DRAW
  );

  return rb_self;
}

VALUE rb_mVismeit_cElementArrayBuffer_initialize(
  const VALUE rb_self,
  const VALUE rb_data
)
{
  Check_Type(rb_data, T_STRING);

  CDATA_mVismeit_cElementArrayBuffer *cdata_self;
  Data_Get_Struct(rb_self, CDATA_mVismeit_cElementArrayBuffer, cdata_self);

  glGenBuffers(1, &cdata_self->gl_id);

  if (cdata_self->gl_id == 0)
  {
    rb_raise(rb_eRuntimeError, "can not generate buffer object");
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cdata_self->gl_id);

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    RSTRING_LEN(rb_data),
    RSTRING_PTR(rb_data),
    GL_STATIC_DRAW
  );

  return rb_self;
}
