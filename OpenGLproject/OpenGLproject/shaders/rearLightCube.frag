#version 410 core

out vec4 fColor;

uniform int rearLightOn;
uniform int solidView;

void main() 
{    
    if(rearLightOn == 1) {
        if(solidView == 1) {
            fColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            fColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else {
        fColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
