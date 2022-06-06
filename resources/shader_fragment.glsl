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
uniform float Aim;

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

vec4 celShade(vec3 lp){
    vec3 ld = normalize(vec3(lp-vertex_pos));
    float intensity = dot(ld,normalize(vertex_normal));    
    vec4 color2;

    // if (intensity > 0.95)
	// 	color2 = vec4(1.0, 1.0, 1.0, 1.0);
	// else if (intensity > 0.5)
	// 	color2 = vec4(0.9,0.9,0.9,1.0);
	// else if (intensity > 0.25)
	// 	color2 = vec4(0.8,0.8,0.8,1.0);
	// else
	// 	color2 = vec4(0.7,0.7,0.7,1.0);

    if (intensity > 0.95)
        color2 = vec4(1.0, 1.0, 1.0, 1.0);
	else if (intensity > 0.5)
		color2 = vec4(0.8,0.8,0.8,1.0);
	else if (intensity > 0.25)
		color2 = vec4(0.6,0.6,0.6,1.0);
	else
		color2 = vec4(0.4,0.4,0.4,1.0);
    return color2;
}

void main()
{
    vec3 lightpos = vec3(0, 0, -50);//can replace with campos to fix light in one spot
    
    vec4 color2 = celShade(campos*2);
    if(Aim == 1.0) {
        color = vec4(0.5, 0.5, 0.5, 1);
    } else {
        // diffuse coefficient
        float dC = getdiff(campos*2);

        // ambient coefficient
        vec3 amb = MatDif / 10;

        // specular coefficient
        float sC = getspec(campos*2);

        vec3 color3 = amb * MatAmb + dC * MatDif + sC * MatSpec;
        color = vec4(color3, Opacity) * color2;
    }
}