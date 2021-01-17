struct PointLight {
  vec3 position;
  
  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 lightColor;
};

#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform float uShininess;

varying vec3 vNormal;
varying vec3 vPosition;

// test
// uniform vec3 Ka;
// uniform vec3 Kd;
// uniform vec3 Ks;
// uniform vec3 LightPosition;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
  vec3 N = normalize(vNormal);
  vec3 L = normalize(vec3(vec4(pointLights[0].position, 1.0) * viewMatrix) - vPosition);

  vec3 result = vec3(0.0, 0.0, 0.0);
  vec3 V = normalize(-vPosition);

  for(int i=0; i < NR_POINT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], N, vPosition, V);
  }

  gl_FragColor = vec4(result, 1.0) ;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightPosition = vec3(viewMatrix * vec4(light.position, 1.0));
  // vec3 lightDir = normalize(LightPosition - vPosition);
  vec3 lightDir = normalize(lightPosition - fragPos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
  // attenuation
  float distance = length(lightPosition - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
            light.quadratic * (distance * distance));
  // combine results
  // vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
  // vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
  // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

  vec3 ambient  = light.ambient;
  vec3 diffuse  = light.diffuse  * diff;
  vec3 specular = light.specular * spec;

  // ambient *= attenuation;
  // diffuse *= attenuation;
  // specular *= attenuation;
  return (ambient + diffuse + specular) * light.lightColor;
  // return vec3(attenuation, 0.0, 0.0);
}
