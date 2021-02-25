#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 inNormalW;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPosT; 
uniform vec3 lightColorT;
uniform vec3 lightPosD;
uniform vec3 lightColorD;
uniform vec3 cameraPos;


vec4 sRGBSpaceToLinearSpace(vec4 sRGB)
{
	vec3 linear = pow(sRGB.xyz, vec3(2.2));
	return vec4(linear, sRGB.w);
}

vec3 getNormal(bool usingNormalMap)
{
	if(usingNormalMap == false)
		return inNormalW;

	/*vec3 q1 = dFdx(FragPos);
	vec3 q2 = dFdx(FragPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);
	vec3 inTangent = q1*st2.t - q2*st1.t;
	vec3 N = normalize(inNormalW);
	vec3 T = normalize(inTangent);
	vec3 B = cross(N,T);
	mat3 TBN = mat3(T,B,N);
	vec3 outNormal = TBN * normalize(sRGBSpaceToLinearSpace(texture(texture_diffuse1,TexCoords)).xyz * 2.0 - vec3(1.0));
	return outNormal;*/
}

void main()
{    
 // ambient

    float ambientStrength = 0.1;
    float intensity =5.0;
    vec3 V = normalize(cameraPos-FragPos);
	vec3 color = texture(texture_diffuse1,TexCoords).rgb;
	vec3 ambient = ambientStrength * color;	
	vec3 norm = getNormal(false);

    // diffuseT 
    vec3 lightDirT = normalize(lightPosT-FragPos);
    vec3 RT = reflect(-lightDirT, norm);
    float diffT = max(dot(norm,lightDirT),0.0);
    float NdotRT = max(0.0,dot(RT,V));
    vec3 specularT = color * pow(NdotRT, 16.0);
    vec3 diffuseT = diffT * lightColorT * color;
    vec3 resultT = intensity * (diffuseT + specularT);

    // diffuseD
    vec3 lightDirD = normalize(lightPosD-FragPos);
    vec3 RD = reflect(-lightDirD,norm);
    float diffD = max(dot(norm, lightDirD), 0.0);
    float NdotRD = max(0.0,dot(RD,V));
    vec3 specularD = color * pow(NdotRD, 16.0);
    vec3 diffuseD = diffD * color * lightColorD;
    vec3 resultD = intensity * (diffuseD + specularD);

    //FragColor = texture(texture_diffuse1,TexCoords)*(vec4(resultT,1.0)+vec4(resultD,1.0));
    FragColor = (vec4(resultT,1.0)+vec4(resultD,1.0)) + vec4(ambient,1.0);
    //FragColor = texture(texture_diffuse1,TexCoords);
}