#version 330 core

#define POINT_LIGHTS 4

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 objectCentre;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform vec3 lightPos[POINT_LIGHTS];
uniform vec3 lightColor[POINT_LIGHTS];


vec3 CalcPointLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos[index] - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    float distance = length(lightPos[index] - fragPos);
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = lightColor[index] * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = vec3(0.0f, 0.0f, 0.0f) * spec * vec3(texture(texture_diffuse1, TexCoords));

    vec3 lightCentreDir = normalize(lightPos[index]-objectCentre);
    float theta = dot(lightDir, lightCentreDir);
    float cutOff = cos(radians(12.5f));
    float outerCutOff = cos(radians(30.0f));
    float epsilon = outerCutOff - cutOff;
    float intensity = clamp((theta - cutOff)/epsilon, 0.0f, 1.0f);

    float attenuation = 1.0f / (1.0f + 0.35*distance + 0.44*distance*distance);

    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;
    return (ambient + diffuse + specular);
}

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 res = vec3(0.0f, 0.0f, 0.0f); 
    for (int i = 0; i < POINT_LIGHTS; i++){
        res += CalcPointLight(i, norm, FragPos, viewDir);
    }
    FragColor = vec4(res, 1.0f);
}



