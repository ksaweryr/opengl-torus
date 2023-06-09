#version 330 core

const float PI = 3.1415926535897932384626433832795;
uniform float time;
uniform int R;
uniform int r;

out vec4 v_position;

mat4 translate(float x, float y, float z) {
    return mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    );
}

mat4 rotateX(float theta) {
    return mat4(
        1, 0, 0, 0,
        0, cos(theta), -sin(theta), 0,
        0, sin(theta), cos(theta), 0,
        0, 0, 0, 1
    );
}

mat4 rotateY(float theta) {
    return mat4(
        cos(theta), 0, sin(theta), 0,
        0, 1, 0, 0,
        -sin(theta), 0, cos(theta), 0,
        0, 0, 0, 1
    );
}

mat4 rotateZ(float theta) {
    return mat4(
        cos(theta), -sin(theta), 0, 0,
        sin(theta), cos(theta), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

void main() {
    float angle_i = gl_InstanceID / r * 2.0f * PI / R;
    float angle_j = gl_InstanceID % r * 2.0f * PI / r;
    gl_Position = v_position = rotateZ(time * PI / 7.0f) * rotateX(time * PI / 4.0f) * rotateY(angle_i) * translate(0.5f, 0, 0) * rotateZ(angle_j) * vec4(0.0f, 0.15f, 0.0f, 1.0f);
}