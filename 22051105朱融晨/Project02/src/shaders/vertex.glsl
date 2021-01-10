varying vec2 vUv;
varying vec3 Normal;
varying vec3 Position;

// uniform mat4 modelViewMatrix;
// uniform mat4 projectionMatrix;

void main() {
  // uv坐标
  vUv = uv;

  Normal = normalize(normalMatrix * normal);
  Position = vec3(modelViewMatrix * vec4(position, 1.0));

  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
