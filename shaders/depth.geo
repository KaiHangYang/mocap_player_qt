#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadow_mat[6];

out vec4 fragPos; // FragPos from GS (output per emitvertex)

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) {
            fragPos = gl_in[i].gl_Position;
            gl_Position = shadow_mat[face] * fragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
} 
