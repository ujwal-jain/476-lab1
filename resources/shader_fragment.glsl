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
    diff = clamp(diff, 0.5, 1.0);
//    diff = pow(diff, 30.0);
    return diff;
//    return clamp(diff, 0.3, 1.0);
}

void main()
{
    vec4 tcol = texture(tex, vertex_tex);
    color = tcol;
//    color = vec4(1, 0, 1, 1);
//
//    float diff = getdiff(vec3(50, 50, 50));
//    int tune = int (diff*10.0);
////    if(diff < pow(0.75, fraction)) {
////        color = color;
////    } else if(diff < pow(0.5, fraction)) {
////        color.xyz = 0.5 * color.xyz;
////    } else {
////        color.xyz = 0.25 * color.xyz;
////    }
////    if(diff < 0.5) {
////        color.xyz = 0.5 * color.xyz;
////    } else {
////        color.xyz = 0.25 * color.xyz;
////    }
//    color.xyz *= (tune/10.0);
}
