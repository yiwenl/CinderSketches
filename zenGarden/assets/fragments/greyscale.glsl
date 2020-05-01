vec3 greyscale(vec3 color, float strength) {
  float gray = dot(color, vec3(0.299, 0.587, 0.114));
  return mix(color, vec3(gray), strength);
}

vec3 greyscale(vec3 color) {
  return greyscale(color, 1.0);
}

vec4 greyscale(vec4 color) {
  return vec4(greyscale(color.rgb), color.a);
}

vec4 greyscale(vec4 color, float strength) {
  return vec4(greyscale(color.rgb, strength), color.a);
}