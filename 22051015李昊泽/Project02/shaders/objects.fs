#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform sampler2D texture2;

void main(){
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    vec3 objectColor = vec3(texture(texture2, TexCoords));

    float ambientRatio = 0.2;
    vec3 ambient = ambientRatio * objectColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    
    vec3 diffuse = diff * lightColor * objectColor;

    FragColor = vec4(ambient + diffuse, 1.0f);
}