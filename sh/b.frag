#version 120

varying vec3 f_color;

void main(void) {
  gl_FragColor = vec4(1.0 - f_color.r, 1.0 - f_color.g, 1.0 - f_color.b, 1.0);
}
