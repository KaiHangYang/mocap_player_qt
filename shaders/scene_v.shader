#version 330 core
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 color_data;

uniform mat4 mvp;

out vec3 v_color;

void main() {
	gl_Position = mvp * vec4(v_position, 1);
    v_color = color_data;
}
