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
    //diff = pow(diff, 30.0);
    return diff;
//    return clamp(diff, 0.3, 1.0);
}

void main()
{
//    vec4 tcol = texture(tex, vertex_tex);
////    color = tcol;
//    color = vec4(1, 0, 1, 1);
//
//    float diff = getdiff(vec3(50, 50, 50));
    //int tune = int (diff*10.0);
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
    //color.xyz *= (tune/10.0);
//    color = vec4(color.rgb * diff + color.rgb * 0.1, 1.0);

//    float dC = max(0, dot(normal, light));

    // vec3 testLP = vec3(0, 0, -50);
    // float dC1 = getdiff(testLP);
    // vec3 testLP2 = vec3(0, 25, 25);
    // float dC2 = getdiff(testLP2);
    // float dC = (dC1 + dC2);
    float dC = getdiff(campos*2);


//    color = vec4(dC, dC, dC, Opacity);
    vec3 amb = MatDif / 10;

    color = vec4((amb + dC*MatDif), Opacity);


//    color = vec4(vec3(dC), Opacity);
//    color=vec4(MatDif * MatShine, Opacity);
}