#version 330 core

in vec4 position;
out vec4 FragColor;

void main() {
    float x = (position.x + 1.0f) / 2;
    float y = (position.y + 1.0f) / 2;
    float z = gl_FragCoord.z;
    float intensity = 1.0f - z;
    FragColor = vec4(x * intensity, y * intensity, x * (1 - y) * intensity, 1.0f);
}