#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;


uniform float specularStrength;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;


uniform sampler2D texture1;

void main(){
    vec3 ambient = ambientStrength * lightColor * texture(texture1, TexCoords).rgb;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);


    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor * texture(texture1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = pow( max( dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = spec * lightColor * specularStrength;


    // result color;

    vec3 result = (specular + ambient + diffuse);
    FragColor = vec4(result, 1.0f);


}