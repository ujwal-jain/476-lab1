#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;

float getdiff(vec3 lp) {
    vec3 n = normalize(vertex_normal);
    vec3 ld = vec3(lp-vertex_pos);
    float diff = dot(n, ld);
    return clamp(diff, 0.3, 1.0);
}

void main()
{
    vec4 tcol = texture(tex, vertex_tex*10.);
    color = tcol;
    color *= 0.3;
    color.a = 1;
    color = vec4(1, 1, 1, 1);

    float fraction = 1.0f;
    float diff = getdiff(vec3(100, 100, 100));

    if(diff < pow(0.75, fraction)) {
        color = color;
    } else if(diff < pow(0.5, fraction)) {
        color.xyz = 0.5 * color.xyz;
    } else {
        color.xyz = 0.25 * color.xyz;
    }
    color.xyz *= diff;
}
