#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform int solidView;

//components
vec3 ambient;
uniform float ambientStrength;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

//spotlight
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;

//rear light
uniform vec3 rearLightPos;
uniform int rearLightOn;

float constant = 1.0f; 
float linear = 0.35f; 
float quadratic = 0.44;

//fog
uniform float fogDensity;

float computeAttenuation(vec3 lightPos, vec4 fPosEye) {
    //compute distance to light 
    vec3 lightPosEye = vec3(view * vec4(lightPos, 1.0));
    float dist = length(lightPosEye - fPosEye.xyz); 
    //compute attenuation 
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist)); 

    return att;
}

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;
    vec3 ambientPointLight = 0.6 * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

    float att1 = computeAttenuation(lightPos1, fPosEye);
    float att2 = computeAttenuation(lightPos2, fPosEye);
    float att3 = computeAttenuation(lightPos3, fPosEye);
    float att4 = computeAttenuation(lightPos4, fPosEye);

    float att = max(att1, max(att2, max(att3, att4)));

    if(rearLightOn == 1) {
        vec3 lightPosEye = vec3(view * vec4(rearLightPos, 1.0));
        float dist = length(lightPosEye - fPosEye.xyz); 
        float auxAtt = 1.0f / (1.0f + 0.7f * dist + 1.8f * (dist * dist));

        att = max(att, auxAtt);
    }
   
    ambient = 0.5 * ambient + 0.5 * ambientPointLight * att; 
    diffuse *= att; 
    specular *= att;
}

float computeShadow() {
	// perform perspective divide 
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 

	// Transform to [0,1] range 
	normalizedCoords = normalizedCoords * 0.5 + 0.5; 

	// Get closest depth value from light's perspective 
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r; 

	// Get depth of current fragment from light's perspective 
	if (normalizedCoords.z > 1.0f) 
		return 0.0f; 
	float currentDepth = normalizedCoords.z; 

	// Check whether current frag pos is in shadow 
	float bias = 0.005f; 
	float shadow = currentDepth - bias > closestDepth  ? 1.0f : 0.0f; 

	return shadow;
}

float computeFog() 
{ 
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

    float fragmentDistance = length(fPosEye); 
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
    return clamp(fogFactor, 0.0f, 1.0f); 
} 

void main() 
{
    float shadow = computeShadow(); 

    computeDirLight();

    vec3 color = vec3(0.2, 0.2, 0.2);

    if(solidView == 0) {
        //compute final vertex color
        vec3 auxColor = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
        
        //fog 
        float fogFactor = computeFog(); 
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); 
        color = fogColor * (1.0 - fogFactor) + auxColor * fogFactor;
    }
    else {
        color = min((ambient + (1.0f - shadow) * diffuse) * color + (1.0f - shadow) * specular * color, 1.0f);
    }

    fColor = vec4(color, 1.0f);
}
