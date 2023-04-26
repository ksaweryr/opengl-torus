#version 330 core

out vec4 FragColor;
uniform int width;
uniform int height;

void main() {
    float x = gl_FragCoord.x / width;
    float y = gl_FragCoord.y / height;
    float z = gl_FragCoord.z;
    float intensity = 1.0f - z;
    FragColor = vec4(x * intensity, y * intensity, x * (1 - y) * intensity, 1.0f);
}