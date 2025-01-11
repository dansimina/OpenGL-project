#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform vec3 lightColor; 

uniform int solidView;

vec3 ambient;
uniform float ambientStrength;

void main()
{
    if(solidView == 0) {
        ambient = ambientStrength * lightColor;
        color = texture(skybox, textureCoordinates);
        color = min(vec4(ambient, 1.0f) * color, 1.0f); 
    }
    else {
        color = vec4(0.8, 0.8, 0.8, 1.0);
    }
}
