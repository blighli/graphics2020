varying vec3 Normal;
varying vec3 Position;
varying vec2 vUv;

uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform vec4 LightPosition;
uniform vec4 LightIntensity;
uniform float Shininess;
uniform sampler2D map;

uniform float time;

void main() {
  vec3 N = normalize(Normal);
  vec3 L = normalize(vec3(LightPosition) - Position);

  // Lambert's cosine law
  float lambertian = max(dot(N, L), 0.0);
  float specular = 0.0;
  if(lambertian > 0.0) {
    vec3 R = reflect(-L, N);
    vec3 V = normalize(-Position);
    
    float specAngle = max(dot(R, V), 0.0);
    specular = pow(specAngle, Shininess);
  }

  vec4 phong = LightIntensity * vec4(Ka + Kd * lambertian + Ks * specular);

  // float lightX =  cos(time);
  // float lightY =  sin(time);
  // float shadowValue = clamp(1.0 - dot(vec3(lightX, lightY, 0.0), Normal), 0.0, 1.0);

  float shadowValue = clamp(1.0 - dot(LightPosition.xyz, Normal), 0.0, 1.0);

  gl_FragColor = vec4(phong.xyz * shadowValue, 1.0) * texture2D(map, vUv);

}