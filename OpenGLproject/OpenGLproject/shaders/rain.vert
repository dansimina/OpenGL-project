#version 410 core
in vec3 position;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);

    gl_PointSize = 20.0f;
}