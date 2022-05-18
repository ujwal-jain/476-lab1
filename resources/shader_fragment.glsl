#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;
uniform sampler2D tex;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform float Opacity;

float getdiff(vec3 lp) {
    vec3 n = normalize(vertex_normal);
    vec3 ld = normalize(vec3(lp-vertex_pos));
    float diff = dot(n, ld);
    diff = max(diff, 0);
    return diff;
}

float getspec(vec3 lp) {
    vec3 n = normalize(vertex_normal);
    vec3 ld = normalize(vec3(lp-vertex_pos));
    vec3 vd = normalize(vec3(campos-vertex_pos));
    vec3 h = normalize(ld+vd);

    float spec = pow(max(dot(n, h), 0), MatShine);
    return spec;
}

void main()
{
    // diffuse coefficient
    float dC = getdiff(campos*2);

    // ambient coefficient
    vec3 amb = MatDif / 10;

    // specular coefficient
    float sC = getspec(campos*2);

    vec3 color3 = amb * MatAmb + dC * MatDif + sC * MatSpec;
    color = vec4(color3, Opacity);
}