#version 330 core

layout (points) in;
layout (points, max_vertices = 3) out;

in vec4 v_position[];
out vec4 g_position;

void main() {
    g_position = v_position[0];

    gl_Position = g_position;
    EmitVertex();

    gl_Position = vec4(g_position.xyz / 4, 1.0f);
    EmitVertex();

    gl_Position = vec4(-g_position.x / 2, g_position.y / 2, g_position.z / 2, 1.0f);
    EmitVertex();

    EndPrimitive();
}