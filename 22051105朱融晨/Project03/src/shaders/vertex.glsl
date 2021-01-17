varying vec3 vNormal;
varying vec3 vPosition;

// uniform mat4 modelViewMatrix;
// uniform mat4 projectionMatrix;

void main() {
  vNormal = normal;
  vPosition = vec3(modelViewMatrix * vec4(position, 1.0));

  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
