#version 330 core
out vec3 color;
in vec3 v_color;

//uniform sampler2D textureSampler;

void main() {
    //color = texture(textureSampler, UV).rgb;
    color = v_color;
}
