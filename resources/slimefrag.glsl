#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D tex;
uniform int part;

void main()
{
    vec4 tcol = texture(tex, vertex_tex);
    switch(part) {
        case 0:
            color = tcol;
            color.a = 1;
            break;
        case 1:
            color = vec4(0, 0, 0, 0.8);
            break;
        case 2:
            color = vec4(1, 0, 0, 0.8);
            break;
    }
}
