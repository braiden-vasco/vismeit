#include "ext.h"

static VALUE rb_mVismeit;
static VALUE rb_mVismeit_cShader;

static VALUE rb_mVismeit_cShader_alloc(VALUE rb_klass);
static void  rb_mVismeit_cShader_free(rb_mVismeit_cShader_CDATA *free_cdata);

static VALUE rb_mVismeit_cShader_initialize(VALUE rb_self,
                                            VALUE rb_type, VALUE rb_source);

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
